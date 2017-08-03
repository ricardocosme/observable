
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "observable/types.hpp"

#include <boost/signals2.hpp>

namespace observable { namespace member {

struct value_tag {};
    
template<typename Model, typename Tag>
struct value
{
    using member_type = value_tag;
    using model = Model;
    using tag = Tag;
};
    
template<typename Class, typename ValueType, typename Enable = void>
struct value_impl;
    
template<typename Class, typename ValueType>
struct value_impl<Class, ValueType, void>
{
    using Model = ValueType;
    using parent_t = Class;

    value_impl() = default;
    
    value_impl(parent_t& parent, Model& value)
        : _value(&value)
        , _parent(&parent)
    {
    }

    //noexcept to variant assignment?
    value_impl(value_impl&& rhs) noexcept
        : _value(rhs._value)
        , _parent(rhs._parent)
        , _on_change(std::move(rhs._on_change))
    {
    }

    //noexcept to variant assignment?
    value_impl& operator=(value_impl&& rhs) noexcept
    {
        _value = rhs._value;
        _parent = rhs._parent;
        _on_change = std::move(rhs._on_change);
        return *this;
    }

    template<typename T>
    value_impl& operator=(T&& o)
    {
        set(std::forward<T>(o));
        return *this;
    }
    
    void set(Model o)
    {
        *_value = std::move(o);
        if (!_parent->_under_transaction)
        {
            _on_change(*_value);
            _parent->_on_change(*(_parent->_model));
        }
    }
    
    const Model& get() const noexcept
    { return *_value; }
    
    template<typename F>
    boost::signals2::connection on_change(F&& f)
    {
        return _on_change.connect(std::forward<F>(f));
    }
    
    Model* _value{nullptr};
    parent_t* _parent{nullptr};
    boost::signals2::signal<void(const Model&)> _on_change;
};
    
template<typename Class, typename ValueType>
struct value_impl<Class, ValueType, container_tag>
{
    using Model = ValueType;
    using parent_t = Class;
    value_impl(parent_t& parent, Model& value,
               typename Class::Model::const_iterator it)
        : _value(&value)
        , _parent(&parent)
        , _it(it)
    {}
    
    void set(Model o)
    {
        *_value = std::move(o);
        if (!_parent->_parent->_under_transaction)
        {
            _on_change(*_value);
            _parent->_on_value_change(*(_parent->_model), _it);
        }
    }
    
    const Model& get() const noexcept
    { return *_value; }
    
    template<typename F>
    boost::signals2::connection on_change(F&& f)
    { return _on_change.connect(std::forward<F>(f)); }
    
    Model* _value{nullptr};
    parent_t* _parent{nullptr};
    boost::signals2::signal<void(const Model&)> _on_change;
    typename Class::Model::const_iterator _it;
};
    
}}
