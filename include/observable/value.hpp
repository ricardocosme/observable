
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "observable/is_observable.hpp"

#include <boost/signals2/signal.hpp>

#include <type_traits>
#include <utility>

namespace observable {
    class parent_observable;                
template<typename Observed_>
class value
{    
    Observed_ _observed;
    boost::signals2::signal<void(const Observed_&)> _on_change;
    friend class mapped_parent_observable;
    friend class parent_observable;
public:    
    using Observed = Observed_;

    value() = default;
    
    value(Observed observed)
        : _observed(std::move(observed))
    {
    }

    value(value&& rhs)
        noexcept(std::is_nothrow_move_constructible<Observed>::value)
        : _observed(std::move(rhs._observed))
        , _on_change(std::move(rhs._on_change))
    {
    }

    value& operator=(value&& rhs)
        noexcept(std::is_nothrow_move_constructible<Observed>::value)
    {
        _observed = std::move(rhs._observed);
        _on_change = std::move(rhs._on_change);
        return *this;
    }

    template<typename T>
    value& operator=(T&& o)
    {
        _observed = std::forward<T>(o);
        _on_change(_observed);
        return *this;
    }
    
    template<typename F>
    boost::signals2::connection on_change(F&& f)
    {
        return _on_change.connect(std::forward<F>(f));
    }

    const Observed& get() const noexcept
    { return _observed; }

    Observed extract()
    {        
        auto o = std::move(_observed);
        _on_change(_observed);
        return o;
    }
};
    
template<typename T>
inline bool operator==(const value<T>& lhs, const value<T>& rhs)
{ return lhs.get() == rhs.get(); }
    
template<typename T>
inline bool operator!=(const value<T>& lhs, const value<T>& rhs)
{ return !(lhs == rhs); }
    
template<typename T>
inline bool operator==(const T& lhs, const value<T>& rhs)
{ return lhs == rhs.get(); }
    
template<typename T>
inline bool operator!=(const T& lhs, const value<T>& rhs)
{ return !(lhs == rhs); }
    
template<typename T>
inline bool operator==(const value<T>& lhs, const T& rhs)
{ return lhs.get() == rhs; }
    
template<typename T>
inline bool operator!=(const value<T>& lhs, const T& rhs)
{ return !(lhs == rhs); }
    
template<typename T>    
struct is_observable<value<T>> : std::true_type
{
    // using type = std::true_type;
};
        
}
