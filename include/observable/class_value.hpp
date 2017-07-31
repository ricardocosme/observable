
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "observable/types.hpp"

#include <boost/signals2.hpp>

namespace observable {

struct value_member_tag {};
    
template<typename T, typename Tag>
struct value
{
    using tag = Tag;
    using type = T;
    using member_type = value_member_tag;
};
    
template<typename Class, typename ValueType, typename Enable = void>
struct value_impl;
    
template<typename Class, typename ValueType>
struct value_impl<Class, ValueType, void>
{
    using type = ValueType;
    using parent_t = Class;

    //TODO
    value_impl() = default;
    
    value_impl(parent_t& parent, type& value)
        : _value(&value)
        , _parent(&parent)
    {
    }

    //?
    value_impl(const value_impl& rhs)
        : _value(rhs._value)
        , _parent(rhs._parent)
    {
        _on_change.connect(rhs._on_change);
    }

    //?
    value_impl& operator=(const value_impl& rhs)
    {
        _value = rhs._value;
        _parent = rhs._parent;
        _on_change.connect(rhs._on_change);
        return *this;
    }
    
    void set(type o)
    {
        *_value = std::move(o);
        if (!_parent->_under_transaction)
        {
            _on_change(*_value);
            _parent->_on_change(*(_parent->_model));
        }
    }
    const type& get() const noexcept
    { return *_value; }
    template<typename F>
    void apply(F&& f)
    {
        f(*_value);
        if (!_parent->_under_transaction)
        {
            _on_change(*_value);
            _parent->_on_change(*(_parent->_model));
        }        
    }
    template<typename F>
    boost::signals2::connection on_change(F&& f)
    {
        return _on_change.connect(std::forward<F>(f));
    }
    type* _value;
    parent_t* _parent;
    boost::signals2::signal<void(const type&)> _on_change;
};
    
template<typename Class, typename ValueType>
struct value_impl<Class, ValueType, container_tag>
{
    using type = ValueType;
    using parent_t = Class;
    value_impl(parent_t& parent, type& value,
               typename Class::type::const_iterator it)
        : _value(&value)
        , _parent(&parent)
        , _it(it)
    {
        auto on_change = _parent->_ref2on_change[&*it].lock();
        if (!on_change)
        {
            auto& ref2on_change = _parent->_ref2on_change;
            auto it_ptr = &*it;
            on_change = std::shared_ptr<boost::signals2::signal<void(const type&)>>
                (new boost::signals2::signal<void(const type&)>(),
                 [&ref2on_change, it_ptr](boost::signals2::signal<void(const type&)> *p)
                 {
                     ref2on_change.erase(it_ptr);
                     delete p;
                 });
            ref2on_change[&*it] = on_change;
        }
        _on_change = on_change;
            
    }
    
    void set(type o)
    {
        *_value = std::move(o);
        if (!_parent->_parent->_under_transaction)
        {
            (*_on_change)(*_value);
            _parent->_on_value_change(*(_parent->_model), _it);
        }
    }
    
    const type& get() const noexcept
    { return *_value; }
    
    template<typename F>
    boost::signals2::connection on_change(F&& f)
    {
        return _on_change->connect(std::forward<F>(f));
    }
    type* _value;
    parent_t* _parent;
    std::shared_ptr<boost::signals2::signal<void(const type&)>> _on_change;
    typename Class::type::const_iterator _it;
};
    
}
