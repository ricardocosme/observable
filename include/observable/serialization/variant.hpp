#pragma once

#include <observable/variant.hpp>

#include <boost/mpl/front.hpp>
#include <boost/mpl/pop_front.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/empty.hpp>

#include <boost/serialization/split_free.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>

namespace boost { namespace serialization {
        
template<class Archive>
struct variant_save
{
    using result_type = void;
    
    variant_save(Archive& ar)
        : _ar(ar)
    {}
    
    template<class T>
    result_type operator()(const T& value) const
    { _ar << BOOST_SERIALIZATION_NVP(value); }
private:
    Archive& _ar;
};
        
template<class S>
struct observable_variant_impl
{
    struct load_null {
        template<class Archive, class V>
        static void invoke(
            Archive & /*ar*/,
            int /*which*/,
            V & /*v*/,
            const unsigned int /*version*/
        ){}
    };

    struct load_impl
    {
        template<class Archive, class Variant>
        static void invoke(Archive& ar,
                           int which,
                           Variant& variant,
                           unsigned int version)
        {
            if(which == 0)
            {
                using head_type = typename mpl::front<S>::type;
                head_type value;
                ar >> BOOST_SERIALIZATION_NVP(value);
                variant = std::move(value);
                return;
            }
            using type = typename mpl::pop_front<S>::type;
            observable_variant_impl<type>::load(ar, which - 1, variant, version);
        }
    };
    
    template<class Archive, class V>
    static void load(Archive& ar,
                     int which,
                     V& variant,
                     unsigned int version)
    {
        typedef typename mpl::eval_if<mpl::empty<S>,
            mpl::identity<load_null>,
            mpl::identity<load_impl>
        >::type typex;
        typex::invoke(ar, which, variant, version);
    }
};
        
}}

namespace observable { namespace serialization {

struct variant
{        
    template<typename Archive, typename...T>
    static void save(Archive& ar,
                     const observable::variant<T...>& o,
                     unsigned int /*version*/)
    {
        auto& v = o.model(); 
        auto which = v.which();
        ar << BOOST_SERIALIZATION_NVP(which);
        boost::serialization::variant_save<Archive> visitor(ar);
        v.apply_visitor(visitor);
    }
 
    template<typename Archive, typename...T>
    static void load(Archive& ar,
                     observable::variant<T...>& o,
                     unsigned int version)
    {
        int which;
        ar >> BOOST_SERIALIZATION_NVP(which);
        boost::serialization::observable_variant_impl
            <typename observable::variant<T...>::Model::types>::load
            (ar, which, o._variant, version);
    }
};
        
}}

namespace boost { namespace serialization {
                        
template<typename Archive, typename...T>
inline void save(Archive& ar,
                 const observable::variant<T...>& o,
                 unsigned int version)
{
    ::observable::serialization::variant::save(ar, o, version);
}
 
template<typename Archive, typename...T>
inline void load(Archive& ar,
                 observable::variant<T...>& o,
                 unsigned int version)
{
    ::observable::serialization::variant::load(ar, o, version);
}
 
template<typename Archive, typename...T>
inline void serialize(Archive& ar, observable::variant<T...>& o,
                      unsigned int version)
{
    split_free(ar, o, version);
}
    
}}
