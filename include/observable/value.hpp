
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <boost/signals2.hpp>

namespace observable { 
            
template<typename Observed_>
struct value
{
    using Observed = Observed_;

    value() = default;
    
    value(Observed& observed)
        : _observed(&observed)
    {
    }

    //noexcept to variant assignment?
    value(value&& rhs) noexcept
        : _observed(rhs._observed)
        , _on_change(std::move(rhs._on_change))
    {
    }

    //noexcept to variant assignment?
    value& operator=(value&& rhs) noexcept
    {
        _observed = rhs._observed;
        _on_change = std::move(rhs._on_change);
        return *this;
    }

    template<typename T>
    value& operator=(T&& o)
    {
        assign(std::forward<T>(o));
        return *this;
    }
    
    void assign(Observed o)
    {
        *_observed = std::move(o);
        _on_change(*_observed);
    }
    
    const Observed& get() const noexcept
    { return *_observed; }
    
    template<typename F>
    boost::signals2::connection on_change(F&& f)
    {
        return _on_change.connect(std::forward<F>(f));
    }
    
    Observed* _observed{nullptr};
    boost::signals2::signal<void(const Observed&)> _on_change;
};
    
}
