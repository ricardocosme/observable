
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <map>
#include <vector>
#include <unordered_set>
#include <boost/variant.hpp>

namespace observable {
    
template<typename Model>
struct is_class : std::false_type {};
    
template<typename Model>
struct is_map : std::false_type {};

//?    
template<typename Key, typename T>
struct is_map<std::map<Key, T>>
    : std::true_type {};
    
template<typename T>
struct is_vector : std::false_type {};
    
template<typename T>
struct is_vector<std::vector<T>>
    : std::true_type {};
            
template<typename T>
struct is_unordered_set : std::false_type {};
    
template<typename T>
struct is_unordered_set<std::unordered_set<T>>
    : std::true_type {};
    
template<typename T>
struct is_variant : std::false_type {};
    
template<BOOST_VARIANT_ENUM_PARAMS(typename T)>
struct is_variant<boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)>>
    : std::true_type {};
}

