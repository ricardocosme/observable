
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <boost/signals2.hpp>

namespace observable { namespace member {

struct unordered_set_tag {};
    
template<typename Model, typename Tag>
struct unordered_set
{
    using member_type = unordered_set_tag;
    using model = Model;
    using tag = Tag;
};
    
template<typename Class, typename Model_>
struct unordered_set_impl
{
    using Model = Model_;
    using parent_t = Class;
    
    unordered_set_impl() = default;
    
    unordered_set_impl(parent_t& parent, Model& value)
        : _model(&value)
        , _parent(&parent)
    {}
    
    typename Model::iterator begin() noexcept
    { return _model->begin(); }

    typename Model::const_iterator cbegin() const noexcept
    { return _model->cbegin(); }

    typename Model::iterator end() noexcept
    { return _model->end(); }
    
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
        if (!_parent->_under_transaction)
        {
            _on_erase(*_model);
            _on_change(*_model);
            _parent->_on_change(*(_parent->_model));
        }
    }
    
    typename Model::iterator erase(typename Model::const_iterator pos)        
    {
        auto it = _model->erase(pos);
        if (!_parent->_under_transaction)
        {
            _on_erase(*_model);
            _on_change(*_model);
            _parent->_on_change(*(_parent->_model));
        }
        return it;
    }
    
    typename Model::iterator erase(typename Model::const_iterator first,
                                  typename Model::const_iterator last)        
    {
        auto it = _model->erase(first, last);
        if (!_parent->_under_transaction)
        {
            _on_erase(*_model);
            _on_change(*_model);
            _parent->_on_change(*(_parent->_model));
        }
        return it;
    }
    
    typename Model::size_type erase(const typename Model::key_type& key)
    {
        auto n = _model->erase(key);
        if (n > 0 && !_parent->_under_transaction)
        {
            _on_erase(*_model);
            _on_change(*_model);
            _parent->_on_change(*(_parent->_model));
        }
        return n;
    }
    
    template<typename... Args>
    std::pair<typename Model::iterator, bool> emplace(Args&&... args)
    {
        auto ret = _model->emplace(std::forward<Args>(args)...);
        if (ret.second && !_parent->_under_transaction)
        {
            _on_insert(*_model);
            _on_change(*_model);
            _parent->_on_change(*(_parent->_model));            
        }
        return ret;
    }
    
    template<typename... Args>
    typename Model::iterator emplace_hint
    (typename Model::const_iterator hint, Args&&... args)
    {
        auto before_size = _model->size();
        auto it = _model->emplace_hint(hint, std::forward<Args>(args)...);
        if (_model->size() != before_size && !_parent->_under_transaction)
        {
            _on_insert(*_model);
            _on_change(*_model);
            _parent->_on_change(*(_parent->_model));            
        }
        return it;
    }

    std::pair<typename Model::iterator, bool> insert
    (const typename Model::value_type& value)
    {
        auto ret = _model->insert(value);
        if (ret.second && !_parent->_under_transaction)
        {
            _on_insert(*_model);
            _on_change(*_model);
            _parent->_on_change(*(_parent->_model));            
        }
        return ret;        
    }
    
    std::pair<typename Model::iterator, bool> insert
    (typename Model::value_type&& value)
    {
        auto ret = _model->insert(std::move(value));
        if (ret.second && !_parent->_under_transaction)
        {
            _on_insert(*_model);
            _on_change(*_model);
            _parent->_on_change(*(_parent->_model));            
        }
        return ret;        
    }
    
    typename Model::iterator insert_hint
    (typename Model::const_iterator hint,
     const typename Model::value_type& value)
    {
        auto before_size = _model->size();
        auto it = _model->insert(hint, value);
        if (_model->size() != before_size && !_parent->_under_transaction)
        {
            _on_insert(*_model);
            _on_change(*_model);
            _parent->_on_change(*(_parent->_model));            
        }
        return it;
    }
    
    typename Model::iterator insert_hint
    (typename Model::const_iterator hint,
     typename Model::value_type&& value)
    {
        auto before_size = _model->size();
        auto it = _model->insert(hint, std::move(value));
        if (_model->size() != before_size && !_parent->_under_transaction)
        {
            _on_insert(*_model);
            _on_change(*_model);
            _parent->_on_change(*(_parent->_model));            
        }
        return it;
    }

    template<typename InputIt>
    void insert(InputIt first, InputIt last)
    {
        auto before_size = _model->size();
        _model->insert(first, last);
        if (_model->size() != before_size && !_parent->_under_transaction)
        {
            _on_insert(*_model);
            _on_change(*_model);
            _parent->_on_change(*(_parent->_model));            
        }
    }
    
    void insert(std::initializer_list<typename Model::value_type> ilist)
    {
        insert(ilist.begin(), ilist.end());
    }
    
    void swap(Model& other)
    {
        _model->swap(other);
        //TODO: check?
        _on_insert(*_model);
        _on_erase(*_model);
        _on_change(*_model);
        _parent->_on_change(*(_parent->_model));            
    }

    typename Model::size_type count
    (const typename Model::key_type& key) const noexcept
    { return _model->count(key); }
    
    typename Model::iterator find
    (const typename Model::key_type& key)
    { return _model->find(key); }
    
    typename Model::const_iterator find
    (const typename Model::key_type& key) const
    { return _model->find(key); }
    
    std::pair<typename Model::iterator,
              typename Model::iterator> equal_range
    (const typename Model::key_type& key)
    { return _model->equal_range(key); }
    
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
    
    const Model& model() const noexcept
    { return *_model; }
    
    Model* _model;
    parent_t* _parent;
    
    boost::signals2::signal<void(const Model&)> _on_erase, _on_insert,
        _on_change;
};
    
}}
