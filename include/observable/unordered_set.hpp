
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <boost/signals2.hpp>

namespace observable {

struct unordered_set_member_tag {};
    
template<typename T, typename Tag>
struct unordered_set
{
    using tag = Tag;
    using type = T;
    using member_type = unordered_set_member_tag;
};
    
template<typename Class, typename Model>
struct unordered_set_impl
{
    using type = Model;
    using parent_t = Class;
    
    unordered_set_impl(parent_t& parent, type& value)
        : _model(value)
        , _parent(parent)
    {}
    // void set(type o)
    // {
    //     _model = std::move(o);
    //     if (!_parent._under_transaction)
    //     {
    //         _on_change(_model);
    //         _parent._on_change(_parent._model);
    //     }
    // }
    // const type& get() const noexcept
    // { return _model; }
    // template<typename F>
    // void apply(F&& f)
    // {
    //     f(_model);
    //     if (!_parent._under_transaction)
    //     {
    //         _on_change(_model);
    //         _parent._on_change(_parent._model);
    //     }        
    // }
    typename type::iterator begin() noexcept
    { return _model.begin(); }

    typename type::const_iterator cbegin() const noexcept
    { return _model.cbegin(); }

    typename type::iterator end() noexcept
    { return _model.end(); }
    
    typename type::const_iterator cend() const noexcept
    { return _model.cend(); }
    
    bool empty() const noexcept
    { return _model.empty(); }
    
    typename type::size_type size() const noexcept
    { return _model.size(); }
    
    typename type::size_type max_size() const noexcept
    { return _model.max_size(); }
        
    void clear() noexcept
    {
        _model.clear();
        if (!_parent._under_transaction)
        {
            _on_erase(_model);
            _on_change(_model);
            _parent._on_change(_parent._model);
        }
    }
    
    typename type::iterator erase(typename type::const_iterator pos)        
    {
        auto it = _model.erase(pos);
        if (!_parent._under_transaction)
        {
            _on_erase(_model);
            _on_change(_model);
            _parent._on_change(_parent._model);
        }
        return it;
    }
    
    typename type::iterator erase(typename type::const_iterator first,
                                  typename type::const_iterator last)        
    {
        auto it = _model.erase(first, last);
        if (!_parent._under_transaction)
        {
            _on_erase(_model);
            _on_change(_model);
            _parent._on_change(_parent._model);
        }
        return it;
    }
    
    typename type::size_type erase(const typename type::key_type& key)
    {
        auto n = _model.erase(key);
        if (n > 0 && !_parent._under_transaction)
        {
            _on_erase(_model);
            _on_change(_model);
            _parent._on_change(_parent._model);
        }
        return n;
    }
    
    template<typename... Args>
    std::pair<typename type::iterator, bool> emplace(Args&&... args)
    {
        auto ret = _model.emplace(std::forward<Args>(args)...);
        if (ret.second && !_parent._under_transaction)
        {
            _on_insert(_model);
            _on_change(_model);
            _parent._on_change(_parent._model);            
        }
        return ret;
    }
    
    template<typename... Args>
    typename type::iterator emplace_hint
    (typename type::const_iterator hint, Args&&... args)
    {
        auto before_size = _model.size();
        auto it = _model.emplace_hint(hint, std::forward<Args>(args)...);
        if (_model.size() != before_size && !_parent._under_transaction)
        {
            _on_insert(_model);
            _on_change(_model);
            _parent._on_change(_parent._model);            
        }
        return it;
    }

    std::pair<typename type::iterator, bool> insert
    (const typename type::value_type& value)
    {
        auto ret = _model.insert(value);
        if (ret.second && !_parent._under_transaction)
        {
            _on_insert(_model);
            _on_change(_model);
            _parent._on_change(_parent._model);            
        }
        return ret;        
    }
    
    std::pair<typename type::iterator, bool> insert
    (typename type::value_type&& value)
    {
        auto ret = _model.insert(std::move(value));
        if (ret.second && !_parent._under_transaction)
        {
            _on_insert(_model);
            _on_change(_model);
            _parent._on_change(_parent._model);            
        }
        return ret;        
    }
    
    typename type::iterator insert_hint
    (typename type::const_iterator hint,
     const typename type::value_type& value)
    {
        auto before_size = _model.size();
        auto it = _model.insert(hint, value);
        if (_model.size() != before_size && !_parent._under_transaction)
        {
            _on_insert(_model);
            _on_change(_model);
            _parent._on_change(_parent._model);            
        }
        return it;
    }
    
    typename type::iterator insert_hint
    (typename type::const_iterator hint,
     typename type::value_type&& value)
    {
        auto before_size = _model.size();
        auto it = _model.insert(hint, std::move(value));
        if (_model.size() != before_size && !_parent._under_transaction)
        {
            _on_insert(_model);
            _on_change(_model);
            _parent._on_change(_parent._model);            
        }
        return it;
    }

    template<typename InputIt>
    void insert(InputIt first, InputIt last)
    {
        auto before_size = _model.size();
        _model.insert(first, last);
        if (_model.size() != before_size && !_parent._under_transaction)
        {
            _on_insert(_model);
            _on_change(_model);
            _parent._on_change(_parent._model);            
        }
    }
    
    void insert(std::initializer_list<typename type::value_type> ilist)
    {
        insert(ilist.begin(), ilist.end());
    }
    
    void swap(type& other)
    {
        _model.swap(other);
        //TODO: check?
        _on_insert(_model);
        _on_erase(_model);
        _on_change(_model);
        _parent._on_change(_parent._model);            
    }

    typename type::size_type count
    (const typename type::key_type& key) const noexcept
    { return _model.count(key); }
    
    typename type::iterator find
    (const typename type::key_type& key)
    { return _model.find(key); }
    
    typename type::const_iterator find
    (const typename type::key_type& key) const
    { return _model.find(key); }
    
    std::pair<typename type::iterator,
              typename type::iterator> equal_range
    (const typename type::key_type& key)
    { return _model.equal_range(key); }
    
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
    
    const Model& model() const noexcept
    { return _model; }
    
    type& _model;
    parent_t& _parent;
    boost::signals2::signal<void(const type&)> _on_erase;
    boost::signals2::signal<void(const type&)> _on_insert;
    boost::signals2::signal<void(const type&)> _on_change;
};
    
}
