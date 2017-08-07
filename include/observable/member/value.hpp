
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "observable/types.hpp"

#include <boost/signals2.hpp>

namespace observable { namespace member {
            
template<typename ValueType>
struct value_impl
{
    using Model = ValueType;

    value_impl() = default;
    
    value_impl(Model& value)
        : _value(&value)
    {
    }

    //noexcept to variant assignment?
    value_impl(value_impl&& rhs) noexcept
        : _value(rhs._value)
        , _on_change(std::move(rhs._on_change))
    {
    }

    //noexcept to variant assignment?
    value_impl& operator=(value_impl&& rhs) noexcept
    {
        _value = rhs._value;
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
        _on_change(*_value);
    }
    
    const Model& get() const noexcept
    { return *_value; }
    
    template<typename F>
    boost::signals2::connection on_change(F&& f)
    {
        return _on_change.connect(std::forward<F>(f));
    }
    
    Model* _value{nullptr};
    boost::signals2::signal<void(const Model&)> _on_change;
};
    
}}
