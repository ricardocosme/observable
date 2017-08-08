
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "observable/types.hpp"

#include <boost/signals2.hpp>

namespace observable { 
            
template<typename ValueType>
struct value
{
    using Model = ValueType;

    value() = default;
    
    value(Model& value)
        : _value(&value)
    {
    }

    //noexcept to variant assignment?
    value(value&& rhs) noexcept
        : _value(rhs._value)
        , _on_change(std::move(rhs._on_change))
    {
    }

    //noexcept to variant assignment?
    value& operator=(value&& rhs) noexcept
    {
        _value = rhs._value;
        _on_change = std::move(rhs._on_change);
        return *this;
    }

    template<typename T>
    value& operator=(T&& o)
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
    
}
