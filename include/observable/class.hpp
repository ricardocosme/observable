
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "observable/is_observable.hpp"

#include <boost/mpl/vector.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>

#include <string>
#include <type_traits>
#include <vector>

namespace observable {

template<typename Class, typename T, T Class::*ptr_>
struct member
{
    using type = T;
    constexpr static T Class::*ptr{ptr_};
};

template<typename Class_>    
struct watch_members
{
    watch_members(Class_& pclass) : _class(pclass) {}
    
    template<typename Member>
    void connect(Member, std::true_type) const
    {
        auto& rclass = static_cast<typename Class_::observable_t&>(_class);
        rclass._conns.push_back
            ((_class.*Member::ptr).on_change
             ([&rclass](const typename Member::type&)
             {rclass._on_change(rclass);}));
    }

    template<typename Member>
    void connect(Member, std::false_type) const
    {
        auto& rclass = static_cast<typename Class_::observable_t&>(_class);   
        rclass._conns.push_back
            ((_class.*Member::ptr).connect
             ([&rclass](const typename Member::type::argument_type&)
             {rclass._on_change(rclass);}));
    }
    
    template<typename Member>
    void operator()(Member) const
    {
        connect(Member{}, is_observable<typename Member::type>{});
    }
    
    Class_& _class;
};

struct class_tag {};
    
template<typename Observable, typename Derived, typename... Member>
class class_ : public Derived {
    using observable_t = Observable;
    using members_t = boost::mpl::vector<Member...>;
    using sig_t = boost::signals2::signal<void(const observable_t&)>;
    sig_t _on_change;
    //TODO: std::array
    std::vector<boost::signals2::scoped_connection> _conns;
    friend struct watch_members<class_<Observable, Derived, Member...>>;
    std::string _tag;

    void watch()
    {
        boost::mpl::for_each<members_t>
            (watch_members<class_<Observable, Derived, Member...>>{*this});
    }
public:
    using category = class_tag;
    using Derived::Derived;
    
    class_()
        : _tag(boost::uuids::to_string
              (boost::uuids::random_generator{}()))
    {
        watch();
    }

    class_(class_&& rhs)
        : Derived(std::move(rhs))
        , _on_change(std::move(rhs._on_change))
        , _tag(std::move(rhs._tag))
    {
        for(auto& conn : rhs._conns)
            conn.disconnect();
        watch();
    }
    
    class_& operator=(class_&& rhs)
    {
        Derived::operator=(std::move(rhs));
        _on_change = std::move(rhs._on_change);
        _tag = std::move(rhs._tag);
        for(auto& conn : rhs._conns)
            conn.disconnect();
        watch();
        return *this;
    }

    template<typename F>
    boost::signals2::connection on_change(F&& f)
    { return _on_change.connect(std::forward<F>(f)); }

    const sig_t& on_change() const noexcept
    { return _on_change; }

    std::string tag() const noexcept
    { return _tag; }    
};
        
template<typename T>        
struct is_observable<
    T,
    typename std::enable_if<
        std::is_same<typename T::category, class_tag>::value
        >::type
    > : std::true_type {};
    
}
