
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "observable/is_observable.hpp"

#include <boost/signals2.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>

namespace observable {
                
template<typename Observed_>
struct value
{    
    using Observed = Observed_;

    value()
    : tag(boost::uuids::to_string(boost::uuids::random_generator{}()))
    {}
    
    value(Observed observed)
        : tag(boost::uuids::to_string(boost::uuids::random_generator{}()))
        , _observed(std::move(observed))
    {
    }

    value(value&& rhs)
        : tag(std::move(rhs.tag))
        , _observed(std::move(rhs._observed))
        , _on_change(std::move(rhs._on_change))
    {
    }

    value& operator=(value&& rhs)
    {
        tag = std::move(rhs.tag);
        _observed = std::move(rhs._observed);
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
        _observed = std::move(o);
        _on_change(_observed);
    }
    
    template<typename F>
    boost::signals2::connection on_change(F&& f)
    {
        return _on_change.connect(std::forward<F>(f));
    }

    const Observed& get() const noexcept
    { return _observed; }
    
    std::string tag;
    
// protected:
    
    Observed _observed;
    boost::signals2::signal<void(const Observed&)> _on_change;
};
    
template<typename T>
bool operator==(const value<T>& lhs, const value<T>& rhs)
{ return lhs.tag == rhs.tag; }
    
template<typename T>
bool operator!=(const value<T>& lhs, const value<T>& rhs)
{ return !(lhs == rhs); }
    
template<typename T>    
struct is_observable<value<T>> : std::true_type {};
        
}
