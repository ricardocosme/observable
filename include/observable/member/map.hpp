
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "observable/member/value.hpp"
#include "observable/member/variant.hpp"
#include "observable/types.hpp"

#include <boost/signals2.hpp>
#include <boost/iterator.hpp>
#include <boost/iterator/reverse_iterator.hpp>
#include <memory>
#include <unordered_map>

namespace observable { namespace member {

struct map_tag {};
    
template<typename Model, template <typename, typename> class ObservableValue, typename Tag>
struct map
{
    using member_type = map_tag;
    using model = Model;
    using tag = Tag;
    using observable_value = ObservableValue<model, tag>;
};

template<typename Model>    
class map_iterator
    : public boost::iterator_adaptor<
        map_iterator<Model>,
        typename Model::Model::iterator,
        std::pair<
            typename Model::Model::key_type,
            std::shared_ptr<typename Model::reference>
        >,
        boost::bidirectional_traversal_tag,
        std::pair<
            typename Model::Model::key_type,
            std::shared_ptr<typename Model::reference>
            >,
        typename Model::Model::difference_type
    >
{
public:
    map_iterator() : map_iterator::iterator_adaptor_()
    {}

    explicit map_iterator
    (Model& model, const typename Model::Model::iterator& it)
        : map_iterator::iterator_adaptor_(it)
        , _model(model)
    {}
private:
    friend class boost::iterator_core_access;
    using value = std::pair<
        typename Model::Model::key_type,
        std::shared_ptr<typename Model::reference>
        >;
    
    value dereference() const
    {
        auto it = this->base_reference();
        //TODO:lvalue ref?
        auto& p = *it;        
        return std::make_pair(p.first, _model.get_reference(it));
    }
    Model& _model;    
};

template<typename Map, typename ObservableValue, typename Enable = void>
struct value_traits;
    
template<typename Map, typename ObservableValue>
struct value_traits<Map, ObservableValue,
                    typename std::enable_if<
                        std::is_same<
                            typename ObservableValue::member_type,
                            member::value_tag
                            >::value
                        >::type>
{
    using type = member::value_impl<Map, typename Map::Model::mapped_type, container_tag>;
};
    
template<typename Map, typename ObservableValue>
struct value_traits<Map, ObservableValue,
                    typename std::enable_if<
                        std::is_same<
                            typename ObservableValue::member_type,
                            member::variant_tag
                            >::value
                        >::type>
{
    using type = variant_impl<Map, typename Map::Model::mapped_type, container_tag>;
};
    
template<typename Class, typename Model_, typename ObservableValue>
struct map_impl
{
    using Model = Model_;
    using parent_t = Class;
    using reference = typename value_traits<
        map_impl<Class, Model, ObservableValue>,
        ObservableValue>::type;
    using iterator = map_iterator<map_impl<Class, Model, ObservableValue>>;
    using difference_type = typename Model::difference_type;

    map_impl() = default;
    
    map_impl(parent_t& parent, Model& value)
        : _model(&value)
        , _parent(&parent)
    {}
    
    iterator begin() noexcept
    { return iterator(*this, _model->begin()); }

    boost::reverse_iterator<iterator> rbegin() noexcept
    {
        return boost::reverse_iterator<iterator>
            (iterator(*this, _model->end()));
    }
    
    typename Model::const_iterator cbegin() const noexcept
    { return _model->cbegin(); }

    typename Model::const_reverse_iterator crbegin() noexcept
    { return _model->crbegin(); }
    
    iterator end() noexcept
    { return iterator(*this, _model->end()); }
    
    boost::reverse_iterator<iterator> rend() noexcept
    {
        return boost::reverse_iterator<iterator>
            (iterator(*this, _model->begin()));
    }
    
    typename Model::const_iterator cend() const noexcept
    { return _model->cend(); }
    
    typename Model::const_reverse_iterator crend() noexcept
    { return _model->crend(); }
    
    bool empty() const noexcept
    { return _model->empty(); }
    
    typename Model::size_type size() const noexcept
    { return _model->size(); }
    
    typename Model::size_type max_size() const noexcept
    { return _model->max_size(); }

    std::shared_ptr<reference> at(const typename Model::key_type& key)
    {
        auto it = _model->lower_bound(key);
        if (it == _model->end() || _model->key_comp()(key, (*it).first))
            throw std::out_of_range("map::at");
        return get_reference(it);
    }
    
    const typename Model::mapped_type& at(const typename Model::key_type& key) const
    { return _model->at(key); }
    
    std::shared_ptr<reference> operator[](const typename Model::key_type& key)
    {
        auto it = _model->lower_bound(key);
        if (it == _model->end() || _model->key_comp()(key, (*it).first))
            it = _model->insert(it, typename Model::value_type
                               (key, typename Model::mapped_type()));
        return get_reference(it);
    }
    
    std::shared_ptr<reference> operator[](typename Model::key_type&& key)
    {
        auto it = _model->lower_bound(key);
        if (it == _model->end() || _model->key_comp()(key, (*it).first))
            it = _model->insert(it, typename Model::value_type
                               (std::move(key), typename Model::mapped_type()));
        return get_reference(it);
    }
    
    void clear() noexcept
    {
        _model->clear();
        if (!_parent->_under_transaction)
        {
            _on_erase(*_model, typename Model::const_iterator{});
            _on_change(*_model);
            _parent->_on_change(*(_parent->_model));
        }
    }
    
    iterator erase(typename Model::const_iterator pos)        
    {
        auto it = _model->erase(pos);
        if (!_parent->_under_transaction)
        {
            _on_erase(*_model, it);
            _on_change(*_model);
            _parent->_on_change(*(_parent->_model));
        }
        return iterator(*this, it);
    }
    
    iterator erase(typename Model::const_iterator first,
                   typename Model::const_iterator last)        
    {
        auto it = _model->erase(first, last);
        if (!_parent->_under_transaction)
        {
            _on_erase(*_model, it);
            _on_change(*_model);
            _parent->_on_change(*(_parent->_model));
        }
        return iterator(*this, it);
    }
    
    typename Model::size_type erase(const typename Model::key_type& key)
    {
        auto rng = _model->equal_range(key);
        auto before_size = _model->size();
        erase(rng.first, rng.second);
        auto n = before_size - _model->size();
        if (n > 0 && !_parent->_under_transaction)
        {
            _on_erase(*_model, rng.second);
            _on_change(*_model);
            _parent->_on_change(*(_parent->_model));
        }
        return n;
    }
    
    template<typename... Args>
    std::pair<iterator, bool> emplace(Args&&... args)
    {
        auto ret = _model->emplace(std::forward<Args>(args)...);
        if (ret.second && !_parent->_under_transaction)
        {
            _on_insert(*_model, ret.first);
            _on_change(*_model);
            _parent->_on_change(*(_parent->_model));            
        }
        return std::make_pair(iterator(*this, ret.first), ret.second);
    }
    
    template<typename... Args>
    iterator emplace_hint
    (typename Model::const_iterator hint, Args&&... args)
    {
        auto before_size = _model->size();
        auto it = _model->emplace_hint(hint, std::forward<Args>(args)...);
        if (_model->size() != before_size && !_parent->_under_transaction)
        {
            _on_insert(*_model, it);
            _on_change(*_model);
            _parent->_on_change(*(_parent->_model));            
        }
        return iterator(*this, it);
    }

    std::pair<iterator, bool> insert
    (const typename Model::value_type& value)
    {
        auto ret = _model->insert(value);
        if (ret.second && !_parent->_under_transaction)
        {
            _on_insert(*_model, ret.first);
            _on_change(*_model);
            _parent->_on_change(*(_parent->_model));            
        }
        return std::make_pair(iterator(*this, ret.first), ret.second);
    }
    
    std::pair<iterator, bool> insert(typename Model::value_type&& value)
    {
        auto ret = _model->insert(std::move(value));
        if (ret.second && !_parent->_under_transaction)
        {
            _on_insert(*_model, ret.first);
            _on_change(*_model);
            _parent->_on_change(*(_parent->_model));            
        }
        return std::make_pair(iterator(*this, ret.first), ret.second);
    }
    
    iterator insert(typename Model::const_iterator hint,
                    const typename Model::value_type& value)
    {
        auto before_size = _model->size();
        auto it = _model->insert(hint, value);
        if (_model->size() != before_size && !_parent->_under_transaction)
        {
            _on_insert(*_model, it);
            _on_change(*_model);
            _parent->_on_change(*(_parent->_model));            
        }
        return iterator(*this, it);
    }
    
    template<typename InputIt>
    void insert(InputIt first, InputIt last)
    {
        auto before_size = _model->size();
        _model->insert(first, last);
        if (_model->size() != before_size && !_parent->_under_transaction)
        {
            _on_insert(*_model, typename Model::const_iterator{}); //TODO
            _on_change(*_model);
            _parent->_on_change(*(_parent->_model));            
        }
    }
    
    void insert(std::initializer_list<typename Model::value_type> ilist)
    { insert(ilist.begin(), ilist.end()); }
    
    void swap(Model& other)
    {
        _model->swap(other);
        //TODO: check?
        _on_insert(*_model, typename Model::const_iterator{});
        _on_erase(*_model, typename Model::const_iterator{});
        _on_change(*_model);
        _parent->_on_change(*(_parent->_model));            
    }

    typename Model::size_type count
    (const typename Model::key_type& key) const noexcept
    { return _model->count(key); }
    
    iterator find(const typename Model::key_type& key)
    { return iterator(*this, _model->find(key)); }
    
    typename Model::const_iterator find
    (const typename Model::key_type& key) const
    { return _model->find(key); }
    
    std::pair<iterator, iterator> equal_range
    (const typename Model::key_type& key)
    {
        auto p = _model->equal_range(key);
        return std::make_pair(iterator(*this, p.first), iterator(*this, p.second));
    }
    
    std::pair<typename Model::const_iterator,
              typename Model::const_iterator> equal_range
    (const typename Model::key_type& key) const
    { return _model->equal_range(key); }
    
    template<typename F>
    boost::signals2::connection on_erase(F&& f)
    { return _on_erase.connect(std::forward<F>(f)); }
    
    template<typename F>
    boost::signals2::connection on_insert(F&& f)
    { return _on_insert.connect(std::forward<F>(f)); }
    
    template<typename F>
    boost::signals2::connection on_change(F&& f)
    { return _on_change.connect(std::forward<F>(f)); }
    
    template<typename F>
    boost::signals2::connection on_value_change(F&& f)
    { return _on_value_change.connect(std::forward<F>(f)); }
    
    const Model& model() const noexcept
    { return *_model; }
    
    Model* _model;
    parent_t* _parent;
    
    boost::signals2::signal<void(const Model&, typename Model::const_iterator)>
    _on_erase, _on_insert, _on_value_change;
    
    boost::signals2::signal<void(const Model&)> _on_change;
    
    std::unordered_map<typename Model::const_iterator::pointer,
                       std::weak_ptr<reference>> _it2observable;
private:
    std::shared_ptr<reference> get_reference(typename Model::iterator it)
    {
        auto observable = _it2observable[&*it].lock();
        if (!observable)
        {
            auto& it2observable = _it2observable;
            auto it_ptr = &*it;
            observable = std::shared_ptr<reference>
                (new reference(*this, (*it).second, it),
                 [&it2observable, it_ptr](reference *p)
                 {
                     it2observable.erase(it_ptr);
                     delete p;
                 });
            it2observable[&*it] = observable;
        }
        return observable;
    }
    friend class map_iterator<map_impl<Class, Model, ObservableValue>>;
};
    
}}
