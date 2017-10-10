#pragma once

#include <observable/unordered_map.hpp>

#include <boost/archive/detail/basic_iarchive.hpp>
#include <boost/serialization/collection_size_type.hpp>
#include <boost/serialization/unordered_collections_save_imp.hpp>
#include <boost/serialization/utility.hpp>

namespace observable { namespace serialization {
    
struct unordered_map
{
    template<typename Archive,
             typename Key,
             typename T>
    static void save(Archive& ar,
                     const observable::unordered_map<Key, T>& o,
                     const unsigned int /*version*/)
    {
        boost::serialization::stl::save_unordered_collection<
            Archive, 
            typename observable::unordered_map<Key, T>::Model
            >(ar, o.model());
    }
 
    template<typename Archive,
             typename Key,
             typename T>
    static void load(Archive& ar,
                     observable::unordered_map<Key, T>& o,
                     const unsigned int /*version*/)
    {
        using namespace boost::serialization;
        
        collection_size_type count;
        collection_size_type bucket_count;
        item_version_type item_version(0);
        auto library_version(ar.get_library_version());
        
        // retrieve number of elements
        ar >> BOOST_SERIALIZATION_NVP(count);
        ar >> BOOST_SERIALIZATION_NVP(bucket_count);
        if(boost::archive::library_version_type(3) < library_version)
            ar >> BOOST_SERIALIZATION_NVP(item_version);

        o.clear();
        o.rehash(bucket_count);
        while(count-- > 0)
        {
            typename observable::unordered_map<Key, T>::value_type elem;
            ar >> make_nvp("item", elem);
            o.insert(std::move(elem));
        }
    }
};
 
}}

namespace boost { namespace serialization {

template<typename Archive,
         typename Key,
         typename T>
inline void save(Archive& ar,
                 const observable::unordered_map<Key, T>& o,
                 const unsigned int version)
{
    ::observable::serialization::unordered_map::save(ar, o, version);
}
 
template<typename Archive,
         typename Key,
         typename T>
inline void load(Archive& ar,
                 observable::unordered_map<Key, T>& o,
                 const unsigned int version)
{
    ::observable::serialization::unordered_map::load(ar, o, version);
}
        
template<typename Archive,
         typename Key,
         typename T>
inline void serialize(Archive& ar,
                      observable::unordered_map<Key, T>& o,
                      const unsigned int version)
{
    split_free(ar, o, version);
}

}}
