
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "observable/types.hpp"

#include <boost/signals2.hpp>
#include <functional>

namespace observable { 

template<typename Observed_>
struct set_get
{
    using Observed = Observed_;
    using Setter = std::function<void(Observed)>;
    using Getter = std::function<const Observed&()>;
    Setter setter;
    Getter getter;
};
    
template<typename SetGet>
struct setter_value
{
    using Model = typename SetGet::Observed;
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
    
    void assign(Model o)
    {
        _setter(std::move(o));
        _on_change(_getter());
    }
    
    const Model& get() const noexcept
    { return _getter(); }
    
    template<typename F>
    boost::signals2::connection on_change(F&& f)
    {
        return _on_change.connect(std::forward<F>(f));
    }
    
    Setter _setter;
    Getter _getter;
    boost::signals2::signal<void(const Model&)> _on_change;
};
    
}
