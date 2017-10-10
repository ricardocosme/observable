#pragma once

#include <observable/value.hpp>

namespace boost { namespace serialization {
        
template<typename Archive, typename T>
inline void save(Archive& ar,
                 const observable::value<T>& o,
                 const unsigned int version)
{
    ar << o.get();
}
 
template<typename Archive, typename T>
inline void load(Archive& ar,
                 observable::value<T>& o,
                 const unsigned int version)
{
    T observed;
    ar >> observed;
    o = std::move(observed);
}
 
template<typename Archive, typename T>
inline void serialize (Archive& ar,
                       observable::value<T>& o,
                       const unsigned int version)
{
    split_free(ar, o, version);
}
    
}}
