
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

namespace observable {

template<typename Model>
struct observable_of;
    
template<typename T>
using observable_of_t = typename observable_of<T>::type;
    
template<typename T>
T& factory(T& o)
{ return o; }
    
}

