
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "observable/traits.hpp"
#include "observable/types.hpp"

#include <boost/signals2.hpp>
#include <functional>

namespace observable { 

template<typename Observed_>
struct setter_value;
    
template<typename Observed>
struct observable_of<
    Observed,
    typename std::enable_if<is_set_get<Observed>::value>::type
>
{
    using type = setter_value<Observed>;
};
    
template<typename SetGet>
struct setter_value
{
    using Observed = typename SetGet::Observed;
    using Setter = typename SetGet::Setter;
    using Getter = typename SetGet::Getter;
    
    setter_value() = default;

    setter_value(SetGet o)
        : _setter(std::move(o.setter))
        , _getter(std::move(o.getter))
    {}
    
    //noexcept to variant assignment?
    setter_value(setter_value&& rhs) noexcept
        : _setter(std::move(rhs._setter))
        , _getter(std::move(rhs._getter))
        , _on_change(std::move(rhs._on_change))
    {
    }

    //noexcept to variant assignment?
    setter_value& operator=(setter_value&& rhs) noexcept
    {
        _setter = std::move(rhs._setter);
        _getter = std::move(rhs._getter);
        _on_change = std::move(rhs._on_change);
        return *this;
    }

    template<typename T>
    setter_value& operator=(T&& o)
    {
        assign(std::forward<T>(o));
        return *this;
    }
    
    void assign(Observed o)
    {
        _setter(std::move(o));
        _on_change(_getter());
    }
    
    const Observed& get() const noexcept
    { return _getter(); }
    
    template<typename F>
    boost::signals2::connection on_change(F&& f)
    {
        return _on_change.connect(std::forward<F>(f));
    }
    
    Setter _setter;
    Getter _getter;
    boost::signals2::signal<void(const Observed&)> _on_change;
};
    
}
