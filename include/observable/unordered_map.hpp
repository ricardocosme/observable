
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "observable/types.hpp"

#include <boost/signals2.hpp>
#include <boost/iterator.hpp>
#include <boost/iterator/reverse_iterator.hpp>
#include <memory>
#include <unordered_map>
#include <iostream>
namespace observable { 

template<typename Model>    
class unordered_map_iterator
    : public boost::iterator_adaptor<
        unordered_map_iterator<Model>,
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
    unordered_map_iterator() : unordered_map_iterator::iterator_adaptor_()
    {}

    explicit unordered_map_iterator
    (Model& model, const typename Model::Model::iterator& it)
        : unordered_map_iterator::iterator_adaptor_(it)
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

template<typename Model_>
struct unordered_map
{
    using Model = Model_;

    using key_type = typename Model::key_type;
    using mapped_type = typename Model::mapped_type;
    using value_type = typename Model::value_type;
    using key_equal = typename Model::key_equal;
    using hasher = typename Model::hasher;
    using reference = observable_of_t<typename Model::mapped_type>;
    using const_reference = typename Model::const_reference;
    using pointer = reference*;
    using const_pointer = typename Model::const_pointer;
    using iterator = unordered_map_iterator<unordered_map<Model>>;
    using const_iterator = typename Model::const_iterator;
    //TODO local_iterator
    using const_local_iterator = typename Model::const_local_iterator;
    using size_type = typename Model::size_type;
    using difference_type = typename Model::difference_type;
    using allocator_type = typename Model::allocator_type;

    unordered_map() = default;
    
    unordered_map(Model& value)
        : _model(&value)
    {}
    
    iterator begin() noexcept
    { return iterator(*this, _model->begin()); }

    typename Model::const_iterator cbegin() const noexcept
    { return _model->cbegin(); }
    
    iterator end() noexcept
    { return iterator(*this, _model->end()); }
    
    typename Model::const_iterator cend() const noexcept
    { return _model->cend(); }
    
    bool empty() const noexcept
    { return _model->empty(); }
    
    typename Model::size_type size() const noexcept
    { return _model->size(); }
    
    typename Model::size_type max_size() const noexcept
    { return _model->max_size(); }

    void clear() noexcept
    {
        _model->clear();
        _on_erase(*_model, value_type{}, typename Model::const_iterator{});
        _on_change(*_model);
    }
    
    std::pair<iterator, bool> insert
    (const typename Model::value_type& value)
    {
        auto ret = _model->insert(value);
        if (ret.second)
        {
            _on_insert(*_model, ret.first);
            _on_change(*_model);
        }
        return std::make_pair(iterator(*this, ret.first), ret.second);
    }
    
    std::pair<iterator, bool> insert(typename Model::value_type&& value)
    {
        auto ret = _model->insert(std::move(value));
        if (ret.second)
        {
            _on_insert(*_model, ret.first);
            _on_change(*_model);
        }
        return std::make_pair(iterator(*this, ret.first), ret.second);
    }
    
    iterator insert(typename Model::const_iterator hint,
                    const typename Model::value_type& value)
    {
        auto before_size = _model->size();
        auto it = _model->insert(hint, value);
        if (_model->size() != before_size)
        {
            _on_insert(*_model, it);
            _on_change(*_model);
        }
        return iterator(*this, it);
    }
    
    template<typename InputIt>
    void insert(InputIt first, InputIt last)
    {
        auto before_size = _model->size();
        _model->insert(first, last);
        if (_model->size() != before_size)
        {
            _on_insert(*_model, typename Model::const_iterator{}); //TODO
            _on_change(*_model);
        }
    }
    
    void insert(std::initializer_list<typename Model::value_type> ilist)
    { insert(ilist.begin(), ilist.end()); }
    
    template<typename... Args>
    std::pair<iterator, bool> emplace(Args&&... args)
    {
        auto ret = _model->emplace(std::forward<Args>(args)...);
        if (ret.second)
        {
            _on_insert(*_model, ret.first);
            _on_change(*_model);
        }
        return std::make_pair(iterator(*this, ret.first), ret.second);
    }
    
    template<typename... Args>
    iterator emplace_hint
    (typename Model::const_iterator hint, Args&&... args)
    {
        auto before_size = _model->size();
        auto it = _model->emplace_hint(hint, std::forward<Args>(args)...);
        if (_model->size() != before_size)
        {
            _on_insert(*_model, it);
            _on_change(*_model);
        }
        return iterator(*this, it);
    }
    
    iterator erase(typename Model::const_iterator pos)        
    {
        auto e = *pos;
        auto it = _model->erase(pos);
        _on_erase(*_model, std::move(e), it);
        _on_change(*_model);
        return iterator(*this, it);
    }
    
    iterator erase(typename Model::const_iterator first,
                   typename Model::const_iterator last)        
    {
        auto it = _model->erase(first, last);
        _on_erase(*_model, value_type{}, it);
        _on_change(*_model);
        return iterator(*this, it);
    }
    
    typename Model::size_type erase(const typename Model::key_type& key)
    {
        auto rng = _model->equal_range(key);
        if(rng.first == _model->end()) return 0;
        auto e = *rng.first;
        auto before_size = _model->size();
        _model->erase(rng.first, rng.second);
        auto n = before_size - _model->size();
        if (n > 0)
        {
            _on_erase(*_model, std::move(e), rng.second);
            _on_change(*_model);
        }
        return n;
    }
    
    void swap(Model& other)
    {
        _model->swap(other);
        //TODO: check?
        _on_insert(*_model, typename Model::const_iterator{});
        _on_erase(*_model, value_type{}, typename Model::const_iterator{});
        _on_change(*_model);
    }
    
    std::shared_ptr<reference> at(const typename Model::key_type& key)
    {
        auto it = _model->find(key);
        if (it == _model->end())
            throw std::out_of_range("unordered_map::at");
        return get_reference(it);
    }
    
    const typename Model::mapped_type&
    at(const typename Model::key_type& key) const
    { return _model->at(key); }
    
    std::shared_ptr<reference> operator[](const typename Model::key_type& key)
    {
        auto it = _model->find(key);
        if (it == _model->end())
            it = _model->insert(it, typename Model::value_type
                               (key, typename Model::mapped_type()));
        return get_reference(it);
    }
    
    std::shared_ptr<reference> operator[](typename Model::key_type&& key)
    {
        auto it = _model->find(key);
        if (it == _model->end())
            it = _model->insert(it, typename Model::value_type
                               (std::move(key), typename Model::mapped_type()));
        return get_reference(it);
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
        return std::make_pair(iterator(*this, p.first),
                              iterator(*this, p.second));
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
    
    const Model& get() const noexcept
    { return *_model; }
    
    Model* _model;
    
    boost::signals2::signal<void(const Model&, typename Model::const_iterator)>
    _on_insert, _on_value_change;
    
    boost::signals2::signal<void(const Model&, value_type, typename Model::const_iterator)>
    _on_erase;
    
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
                (new reference(observable_factory(it->second)),
                 [&it2observable, it_ptr](reference *p)
                 {
                     it2observable.erase(it_ptr);
                     delete p;
                 });
            auto& container = *this;
            observable->_on_change.connect(
                [&container, it](const typename reference::Model&)
                {
                    container._on_value_change(container.get(), it);
                    container._on_change(container.get());
                });
            it2observable[&*it] = observable;
        }
        return observable;
    }
    friend class unordered_map_iterator<unordered_map<Model>>;
};
    
}
