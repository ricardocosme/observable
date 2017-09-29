
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "observable/exp/is_observable.hpp"

#include <boost/signals2.hpp>
#include <functional>

namespace observable { namespace exp {

template<typename Observed,
         typename T,
         void (Observed::*pset)(T),
         T (Observed::*pget)() const>
class set_get
{
    Observed& _observed;    
    boost::signals2::signal<void()> _on_change;
    
public:
    
    set_get() = default;

    //TODO ref
    set_get(Observed o)
        : _observed(o)
    {
    }
    
    set_get(set_get&& rhs)
        : _observed(rhs._observed)
        , _on_change(std::move(rhs._on_change))
    {
    }

    set_get& operator=(set_get&& rhs)
    {
        _observed = std::move(rhs._observed);
        _on_change = std::move(rhs._on_change);
        return *this;
    }

    template<typename U>
    set_get& operator=(U&& o)
    {
        assign(std::forward<U>(o));
        return *this;
    }
    
    void assign(T o)
    {
        (_observed.*pset)(std::move(o));
        _on_change();
    }
    
    T get() const noexcept
    { return (_observed.*pget)(); }
    
    template<typename F>
    boost::signals2::connection on_change(F&& f)
    {
        return _on_change.connect(std::forward<F>(f));
    }    
};
    
// template<typename Observed, typename T, void (Observed::*set)(T), T (Observed::*get)() const>
// struct set_get : set_get_impl<Observed, T, void (Observed::*set)(T), T (Observed::*get)() const>
// {
    
// };
        
// template<typename Observed,
//          typename T>
// struct is_observable<set_get<Observed, T, void (Observed::*set)(T), T (Observed::*get)() const>> : std::true_type {};
        
    }}
