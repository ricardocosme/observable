
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "observable/exp/is_observable.hpp"
#include "observable/exp/traits.hpp"
#include "observable/exp/types.hpp"
#include "observable/exp/value.hpp"

#include <boost/fusion/include/at_key.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/map.hpp>
#include <boost/fusion/include/move.hpp>
#include <boost/fusion/include/pair.hpp>
#include <boost/signals2.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>

#include <array>
#include <type_traits>

namespace observable { namespace exp {

class class_ {
    boost::signals2::signal<void()> _on_change;
public:    
    class_()
        : tag(boost::uuids::to_string
              (boost::uuids::random_generator{}()))
    {}
    
    template<typename F>
    boost::signals2::connection on_change(F&& f)
    { return _on_change.connect(std::forward<F>(f)); }

    const boost::signals2::signal<void()>& on_change() const noexcept
    { return _on_change; }
    
    std::string tag;    
};
// template<typename>
// struct map;
    
// template<typename>
// struct unordered_map;
    
// template<typename>
// struct unordered_set;
    
// template<typename>
// struct variant;
    
// // template<typename>
// // struct vector;

// template<typename Parent>    
// struct set_on_change
// {
//     set_on_change(Parent& p)
//         : _parent(p)
//         , _it(_parent.observable_on_change_conns.begin())
//     {}
    
//     template<typename T>
//     void operator()(T& o) const
//     {
//         auto& parent = _parent;
//         *_it++ = 
//             o.second.on_change(
//             [&parent]()
//             { parent._on_change(); });
//     }
    
//     Parent& _parent;
//     mutable typename Parent::observable_on_change_conns_t::iterator _it;
// };

// template<typename Observed>
// struct observable_of<
//     Observed,
//     typename std::enable_if<is_class<Observed>::value>::type
// >
// {
//     using type = typename is_class<Observed>::type;
// };

    
// template<typename Observed_, typename... Members>
// class class_ 
// {
// public:
//     using Observed = Observed_;
//     using Tag2Observable = boost::fusion::map<
//         boost::fusion::pair<
//             typename Members::second_type,
//             typename Members::first_type
//         >...>;

//     class_()
//     : tag(boost::uuids::to_string(boost::uuids::random_generator{}()))
//     {}
    
//     template<typename... Observeds>
//     class_(Observeds... observeds)
//         : tag(boost::uuids::to_string(boost::uuids::random_generator{}()))
//         , _tag2observable
//           (boost::fusion::pair<
//            typename Members::second_type,
//            typename Members::first_type>
//            (typename Members::first_type(std::move(observeds)))...)
//     {
//         set_on_change<class_<Observed, Members...>> visitor{*this};
//         boost::fusion::for_each(_tag2observable, visitor);
//     }

//     class_(class_&& rhs) 
//         : tag(std::move(rhs.tag))
//         , _tag2observable(std::move(rhs._tag2observable))
//         , _on_change(std::move(rhs._on_change))
//     {
//         rhs.observable_on_change_conns.swap(observable_on_change_conns);
//         set_on_change<class_<Observed, Members...>> visitor{*this};
//         boost::fusion::for_each(_tag2observable, visitor);
//         // boost::fusion::move(std::move(rhs._tag2observable), _tag2observable);        
//     }
    
//     class_& operator=(class_&& rhs)
//     {
//         tag = std::move(rhs.tag);
//         _tag2observable = std::move(rhs._tag2observable);
//         _on_change = std::move(rhs._on_change);
//         rhs.observable_on_change_conns.swap(observable_on_change_conns);        
//         set_on_change<class_<Observed, Members...>> visitor{*this};
//         boost::fusion::for_each(_tag2observable, visitor);
//     //     boost::fusion::move(std::move(rhs._tag2observable), _tag2observable);
//         return *this;
//     }
    
//     template<typename Tag, typename T>
//     void assign(T o)
//     { return boost::fusion::at_key<Tag>(_tag2observable).assign(std::move(o)); }

//     template<typename Tag>
//     auto get() ->
//         decltype(boost::fusion::at_key<Tag>(
//                      std::declval<
//                      typename std::add_lvalue_reference<Tag2Observable>::type>()))
//     { return boost::fusion::at_key<Tag>(_tag2observable); }
    
//     template<typename Tag>
//     auto get() const ->
//         decltype(boost::fusion::at_key<Tag>(
//                      std::declval<
//                      typename std::add_lvalue_reference<const Tag2Observable>::type>()))
//     { return boost::fusion::at_key<Tag>(_tag2observable); }
    
//     template<typename Tag, typename F>
//     boost::signals2::connection on_change(F&& f)
//     {
//         return boost::fusion::at_key<Tag>
//             (_tag2observable).on_change(std::forward<F>(f));
//     }
    
//     template<typename F>
//     boost::signals2::connection on_change(F&& f)
//     { return _on_change.connect(std::forward<F>(f)); }
    
//     std::string tag;
    
// private:
    
//     Tag2Observable _tag2observable;
//     boost::signals2::signal<void()> _on_change;

//     using observable_on_change_conns_t =
//         std::array<boost::signals2::scoped_connection, sizeof...(Members)>;
    
//     observable_on_change_conns_t observable_on_change_conns;
    
//     // template<typename>
//     // friend struct observable::map;
    
//     // template<typename>
//     // friend struct observable::value;
    
//     // template<typename>
//     // friend struct observable::variant;
    
//     // template<typename>
//     // friend struct observable::vector;
    
//     // template<typename>
//     // friend struct observable::unordered_map;
    
//     // template<typename>
//     // friend struct observable::unordered_set;
    
//     template<typename>
//     friend struct set_on_change;
// };

template<>        
struct is_observable<class_> : std::true_type {};
    
}}
