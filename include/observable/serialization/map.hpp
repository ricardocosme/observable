#pragma once

#include <observable/map.hpp>

#include <boost/archive/detail/basic_iarchive.hpp>
#include <boost/serialization/collection_size_type.hpp>
#include <boost/serialization/collections_save_imp.hpp>
#include <boost/serialization/utility.hpp>

namespace observable { namespace serialization {
    
struct map
{
    template<typename Archive,
             typename Key,
             typename T,
             typename Compare,
             typename Allocator,
             template <typename, typename, typename, typename> class Model>
    static void save(Archive& ar,
                     const observable::map<Key, T, Compare, Allocator, Model>& o,
                     const unsigned int /*version*/)
    {
        boost::serialization::stl::save_collection<
            Archive, 
            typename observable::map<Key, T, Compare, Allocator, Model>::Model
            >(ar, o.model());
    }
 
    template<typename Archive,
             typename Key,
             typename T,
             typename Compare,
             typename Allocator,
             template <typename, typename, typename, typename> class Model>
    static void load(Archive& ar,
                     observable::map<Key, T, Compare, Allocator, Model>& o,
                     const unsigned int /*version*/)
    {
        o.clear();
        auto library_version(ar.get_library_version());
        
        using namespace boost::serialization;
        
        // retrieve number of elements
        item_version_type item_version(0);
        collection_size_type count;
        ar >> BOOST_SERIALIZATION_NVP(count);

        if(boost::archive::library_version_type(3) < library_version)
            ar >> BOOST_SERIALIZATION_NVP(item_version);

        using Container = observable::map<Key, T, Compare, Allocator, Model>;
        typename Container::iterator hint;
        hint = o.begin();
        while(count-- > 0)
        {
            typename Container::value_type elem;
            ar >> make_nvp("item", elem);
            hint = o.insert(hint, std::move(elem));
        }
    }
};
 
}}

namespace boost { namespace serialization {

template<typename Archive,
         typename Key,
         typename T,
         typename Compare,
         typename Allocator,
         template <typename, typename, typename, typename> class Model>
inline void save(Archive& ar,
                 const observable::map<Key, T, Compare, Allocator, Model>& o,
                 const unsigned int version)
{
    ::observable::serialization::map::save(ar, o, version);
}
 
template<typename Archive,
         typename Key,
         typename T,
         typename Compare,
         typename Allocator,
         template <typename, typename, typename, typename> class Model>
inline void load(Archive& ar,
                 observable::map<Key, T, Compare, Allocator, Model>& o,
                 const unsigned int version)
{
    ::observable::serialization::map::load(ar, o, version);
}
        
template<typename Archive,
         typename Key,
         typename T,
         typename Compare,
         typename Allocator,
         template <typename, typename, typename, typename> class Model>
inline void serialize(Archive& ar,
                      observable::map<Key, T, Compare, Allocator, Model>& o,
                      const unsigned int version)
{
    split_free(ar, o, version);
}

}}
