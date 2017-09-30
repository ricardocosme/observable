
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "observable/value.hpp"

#include <type_traits>
#include <utility>

namespace observable {

template<typename Observed, typename Enable = void>
struct observable_of
{ using type = value<Observed>; };
    
template<typename T>
using observable_of_t = typename observable_of<T>::type;
        
}

template<typename T>
observable::observable_of_t<typename std::decay<T>::type>
observable_factory(T o)
{ return std::move(o); }
