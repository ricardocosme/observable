
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "observable/types.hpp"

#include <boost/signals2.hpp>
#include <boost/variant.hpp>

namespace observable {

struct variant_member_tag {};
    
template<typename T, typename Tag>
struct variant
{
    using tag = Tag;
    using type = T;
    using member_type = variant_member_tag;
};
    
template<typename Class, typename Model, typename Enable = void>
struct variant_impl;
    
template<typename Class, typename Model>
struct variant_impl<Class, Model, void>
{
    using type = Model;
    using parent_t = Class;
    variant_impl(parent_t& parent, type& value)
        : _value(value)
        , _parent(parent)
    {}
    void set(type o)
    {
        _value = std::move(o);
        if (!_parent._under_transaction)
        {
            _on_change(_value);
            _parent._on_change(_parent._model);
        }
    }
    const type& get() const noexcept
    { return _value; }
    
    template<typename Visitor>
    void apply_visitor(Visitor&& visitor)
    {
        boost::apply_visitor(std::forward<Visitor>(visitor), _value);
    }
    
    template<typename F>
    boost::signals2::connection on_change(F&& f)
    {
        return _on_change.connect(std::forward<F>(f));
    }
    type& _value;
    parent_t& _parent;
    boost::signals2::signal<void(const type&)> _on_change;
};
    
template<typename Class, typename Model>
struct variant_impl<Class, Model, container_tag>
{
    using type = Model;
    using parent_t = Class;
    variant_impl(parent_t& parent, type& value,
               typename Class::type::const_iterator it)
        : _value(value)
        , _parent(parent)
        , _on_change(std::make_shared<boost::signals2::signal<void(const type&)>>())
        , _it(it)
    {}
    
    void set(type o)
    {
        _value = std::move(o);
        if (!_parent._parent._under_transaction)
        {
            (*_on_change)(_value);
            _parent._on_value_change(_parent._model, _it);
        }
    }
    
    const type& get() const noexcept
    { return _value; }    
    
    template<typename Visitor>
    void apply_visitor(Visitor&& visitor)
    {
        boost::apply_visitor(std::forward<Visitor>(visitor), _value);
    }
    
    template<typename F>
    boost::signals2::connection on_change(F&& f)
    {
        return _on_change->connect(std::forward<F>(f));
    }
    type& _value;
    parent_t& _parent;
    std::shared_ptr<boost::signals2::signal<void(const type&)>> _on_change;
    typename Class::type::const_iterator _it;
};
    
}
