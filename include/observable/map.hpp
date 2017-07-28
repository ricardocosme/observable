
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "observable/class_value.hpp"
#include "observable/types.hpp"

#include <boost/signals2.hpp>
#include <boost/iterator.hpp>

namespace observable {

struct map_member_tag {};
    
template<typename T, typename Tag>
struct map
{
    using tag = Tag;
    using type = T;
    using member_type = map_member_tag;
};

template<typename Model>    
class map_iterator
    : public boost::iterator_adaptor<
        map_iterator<Model>,
        typename Model::type::iterator,
        std::pair<
            typename Model::type::key_type,
            typename Model::reference
        >,
        boost::bidirectional_traversal_tag,
        std::pair<
            typename Model::type::key_type,
            typename Model::reference
            >,
        typename Model::type::difference_type
    >
{
public:
    map_iterator() : map_iterator::iterator_adaptor_()
    {}

    explicit map_iterator
    (Model& model, const typename Model::type::iterator& it)
        : map_iterator::iterator_adaptor_(it)
        , _model(model)
    {}
private:
    friend class boost::iterator_core_access;
    using value = std::pair<
        typename Model::type::key_type,
        typename Model::reference
        >;
    
    value dereference() const
    {
        auto it = this->base_reference();
        //TODO:lvalue ref?
        auto& p = *it;
        return std::make_pair(p.first, typename Model::reference(_model, p.second, it));
    }
    Model& _model;    
};
    
template<typename Class, typename Model>
struct map_impl
{
    using type = Model;
    using parent_t = Class;
    using reference = value_impl<map_impl<Class, Model>, typename type::mapped_type, container_tag>;
    using iterator = map_iterator<map_impl<Class, Model>>;
    using difference_type = typename type::difference_type;
    
    map_impl(parent_t& parent, type& value)
        : _model(value)
        , _parent(parent)
    {}
    
    iterator begin() noexcept
    { return iterator(*this, _model.begin()); }

    typename type::const_iterator cbegin() const noexcept
    { return _model.cbegin(); }

    iterator end() noexcept
    { return iterator(*this, _model.end()); }
    
    typename type::const_iterator cend() const noexcept
    { return _model.cend(); }
    
    bool empty() const noexcept
    { return _model.empty(); }
    
    typename type::size_type size() const noexcept
    { return _model.size(); }
    
    typename type::size_type max_size() const noexcept
    { return _model.max_size(); }

    reference at(const typename type::key_type& key)
    {
        auto it = _model.lower_bound(key);
        if (it == _model.end() || _model.key_comp()(key, (*it).first))
            throw std::out_of_range("map::at");
        return reference(*this, (*it).second, it);
    }
    
    const typename type::mapped_type& at(const typename type::key_type& key) const
    { return _model.at(key); }
    
    reference operator[](const typename type::key_type& key)
    {
        auto it = _model.lower_bound(key);
        if (it == _model.end() || _model.key_comp()(key, (*it).first))
            it = _model.insert(it, typename type::value_type
                               (key, typename type::mapped_type()));
        return reference(*this, (*it).second, it);
    }
    
    reference operator[](typename type::key_type&& key)
    {
        auto it = _model.lower_bound(key);
        if (it == _model.end() || _model.key_comp()(key, (*it).first))
            it = _model.insert(it, typename type::value_type
                               (std::move(key), typename type::mapped_type()));
        return reference(*this, (*it).second, it);
    }
    
    void clear() noexcept
    {
        _model.clear();
        if (!_parent._under_transaction)
        {
            _on_erase(_model, typename type::const_iterator{});
            _on_change(_model);
            _parent._on_change(_parent._model);
        }
    }
    
    iterator erase(typename type::const_iterator pos)        
    {
        auto it = _model.erase(pos);
        if (!_parent._under_transaction)
        {
            _on_erase(_model, it);
            _on_change(_model);
            _parent._on_change(_parent._model);
        }
        return iterator(*this, it);
    }
    
    iterator erase(typename type::const_iterator first,
                   typename type::const_iterator last)        
    {
        auto it = _model.erase(first, last);
        if (!_parent._under_transaction)
        {
            _on_erase(_model, it);
            _on_change(_model);
            _parent._on_change(_parent._model);
        }
        return iterator(*this, it);
    }
    
    typename type::size_type erase(const typename type::key_type& key)
    {
        auto rng = _model.equal_range(key);
        auto before_size = _model.size();
        erase(rng.first, rng.second);
        auto n = before_size - _model.size();
        if (n > 0 && !_parent._under_transaction)
        {
            _on_erase(_model, rng.second);
            _on_change(_model);
            _parent._on_change(_parent._model);
        }
        return n;
    }
    
    template<typename... Args>
    std::pair<iterator, bool> emplace(Args&&... args)
    {
        auto ret = _model.emplace(std::forward<Args>(args)...);
        if (ret.second && !_parent._under_transaction)
        {
            _on_insert(_model, ret.first);
            _on_change(_model);
            _parent._on_change(_parent._model);            
        }
        return std::make_pair(iterator(*this, ret.first), ret.second);
    }
    
    template<typename... Args>
    iterator emplace_hint
    (typename type::const_iterator hint, Args&&... args)
    {
        auto before_size = _model.size();
        auto it = _model.emplace_hint(hint, std::forward<Args>(args)...);
        if (_model.size() != before_size && !_parent._under_transaction)
        {
            _on_insert(_model, it);
            _on_change(_model);
            _parent._on_change(_parent._model);            
        }
        return iterator(*this, it);
    }

    std::pair<iterator, bool> insert
    (const typename type::value_type& value)
    {
        auto ret = _model.insert(value);
        if (ret.second && !_parent._under_transaction)
        {
            _on_insert(_model, ret.first);
            _on_change(_model);
            _parent._on_change(_parent._model);            
        }
        return std::make_pair(iterator(*this, ret.first), ret.second);
    }
    
    std::pair<iterator, bool> insert(typename type::value_type&& value)
    {
        auto ret = _model.insert(std::move(value));
        if (ret.second && !_parent._under_transaction)
        {
            _on_insert(_model, ret.first);
            _on_change(_model);
            _parent._on_change(_parent._model);            
        }
        return std::make_pair(iterator(*this, ret.first), ret.second);
    }
    
    iterator insert(typename type::const_iterator hint,
                    const typename type::value_type& value)
    {
        auto before_size = _model.size();
        auto it = _model.insert(hint, value);
        if (_model.size() != before_size && !_parent._under_transaction)
        {
            _on_insert(_model, it);
            _on_change(_model);
            _parent._on_change(_parent._model);            
        }
        return iterator(*this, it);
    }
    
    template<typename InputIt>
    void insert(InputIt first, InputIt last)
    {
        auto before_size = _model.size();
        _model.insert(first, last);
        if (_model.size() != before_size && !_parent._under_transaction)
        {
            _on_insert(_model, typename type::const_iterator{}); //TODO
            _on_change(_model);
            _parent._on_change(_parent._model);            
        }
    }
    
    void insert(std::initializer_list<typename type::value_type> ilist)
    { insert(ilist.begin(), ilist.end()); }
    
    void swap(type& other)
    {
        _model.swap(other);
        //TODO: check?
        _on_insert(_model, typename type::const_iterator{});
        _on_erase(_model, typename type::const_iterator{});
        _on_change(_model);
        _parent._on_change(_parent._model);            
    }

    typename type::size_type count
    (const typename type::key_type& key) const noexcept
    { return _model.count(key); }
    
    iterator find(const typename type::key_type& key)
    { return iterator(*this, _model.find(key)); }
    
    typename type::const_iterator find
    (const typename type::key_type& key) const
    { return _model.find(key); }
    
    std::pair<iterator, iterator> equal_range
    (const typename type::key_type& key)
    {
        auto p = _model.equal_range(key);
        return std::make_pair(iterator(*this, p.first), iterator(*this, p.second));
    }
    
    std::pair<typename type::const_iterator,
              typename type::const_iterator> equal_range
    (const typename type::key_type& key) const
    { return _model.equal_range(key); }
    
    template<typename F>
    boost::signals2::connection on_erase(F&& f)
    {
        return _on_erase.connect(std::forward<F>(f));
    }
    
    template<typename F>
    boost::signals2::connection on_insert(F&& f)
    {
        return _on_insert.connect(std::forward<F>(f));
    }
    
    template<typename F>
    boost::signals2::connection on_change(F&& f)
    {
        return _on_change.connect(std::forward<F>(f));
    }
    
    template<typename F>
    boost::signals2::connection on_value_change(F&& f)
    {
        return _on_value_change.connect(std::forward<F>(f));
    }
    
    const Model& model() const noexcept
    { return _model; }
    
    type& _model;
    parent_t& _parent;
    boost::signals2::signal<
        void(const type&, typename type::const_iterator)> _on_erase;
    boost::signals2::signal<
        void(const type&, typename type::const_iterator)> _on_insert;
    boost::signals2::signal<
        void(const type&, typename type::const_iterator)> _on_value_change;
    boost::signals2::signal<void(const type&)> _on_change;
};
    
}
