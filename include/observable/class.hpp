
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "observable/class_value.hpp"
#include "observable/unordered_set.hpp"
#include "observable/map.hpp"
#include "observable/member_class.hpp"
#include <boost/signals2.hpp>
#include <boost/fusion/include/map.hpp>
#include <boost/fusion/include/at_key.hpp>
#include <boost/fusion/include/make_map.hpp>
#include <boost/fusion/include/pair.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/move.hpp>
#include <type_traits>

namespace observable {

template<typename>
class scoped_on_change_t;
        
template<typename Class, typename Member, typename Enable = void>
struct member_traits;

template<typename Class, typename Member>
struct member_traits<
    Class,
    Member,
    typename std::enable_if<
        std::is_same<
            typename Member::member_type,
            value_member_tag
        >::value
    >::type
>
{
    using type = value_impl<Class, typename Member::type>;
};
        
template<typename Class, typename Member>
struct member_traits<
    Class,
    Member,
    typename std::enable_if<
        std::is_same<
            typename Member::member_type,
            unordered_set_member_tag
        >::value
    >::type
>
{
    using type = unordered_set_impl<Class, typename Member::type>;
};
    
template<typename Class, typename Member>
struct member_traits<
    Class,
    Member,
    typename std::enable_if<
        std::is_same<
            typename Member::member_type,
            map_member_tag
        >::value
    >::type
>
{
    using type = map_impl<Class, typename Member::type, typename Member::observable_value>;
};
        
template<typename Class, typename Member>
struct member_traits<
    Class,
    Member,
    typename std::enable_if<
        std::is_same<
            typename Member::member_type,
            memberclass_member_tag
        >::value
    >::type
>
{
    using type = memberclass_impl<Class, typename Member::type, typename Member::observable>;
};

template<typename Class, typename Member>
struct member_traits<
    Class,
    Member,
    typename std::enable_if<
        std::is_same<
            typename Member::member_type,
            variant_member_tag
        >::value
    >::type
>
{
    using type = variant_impl<Class, typename Member::type>;
};
    
template<typename Parent>    
struct set_parent
{
    set_parent(Parent& p) : _parent(p) {}
    
    template<typename T>
    void operator()(T& o) const
    {
        o.second._parent = &_parent;
    }
    
    Parent& _parent;
};
    
template<typename Model_, typename... Members>
class class_ 
{    
public:
    using Model = Model_;
    using Tag2Member = boost::fusion::map<
        boost::fusion::pair<
            typename Members::tag,
            typename member_traits<class_<Model, Members...>, Members>::type
        >...>;
    
    class_(Model& model,
           typename Members::type&... args)
        : _model(&model)
        , tag2member(boost::fusion::pair<
                     typename Members::tag,
                     typename member_traits<class_<Model, Members...>, Members>::type>
                     (typename member_traits<class_<Model, Members...>, Members>::type
                      (*this, args))...)
    {
    }

    class_(class_&& rhs)
        : _on_change(std::move(rhs._on_change))
        , _under_transaction(rhs._under_transaction)
        , _model(rhs._model)
        , tag2member(std::move(rhs.tag2member))
    {
        for_each(tag2member, set_parent<class_<Model_, Members...>>{*this});
    }
    
    class_& operator=(class_&& rhs)
    {
        _on_change = std::move(rhs._on_change);
        _under_transaction = rhs._under_transaction;
        _model = rhs._model;
        boost::fusion::move(std::move(rhs.tag2member), tag2member);
        for_each(tag2member, set_parent<class_<Model_, Members...>>{*this});
        return *this;
    }
    
    template<typename M, typename V>
    void set(V o)
    {
        return boost::fusion::at_key<M>(tag2member).set(std::move(o));
    }

    template<typename M>
    auto get() ->
        decltype(boost::fusion::at_key<M>(
                     std::declval<typename std::add_lvalue_reference<Tag2Member>::type>()))
    { return boost::fusion::at_key<M>(tag2member); }
    
    template<typename M, typename F>
    void apply(F&& f)
    {
        return boost::fusion::at_key<M>(tag2member).apply(std::forward<F>(f));
    }
    
    template<typename M, typename F>
    boost::signals2::connection on_change(F&& f)
    {
        return boost::fusion::at_key<M>(tag2member).on_change(std::forward<F>(f));
    }
    
    template<typename F>
    boost::signals2::connection on_change(F&& f)
    {
        return _on_change.connect(std::forward<F>(f));
    }
    
    const Model& model() const noexcept
    { return *_model; }
    
private:    
    boost::signals2::signal<void(const Model&)> _on_change;
    bool _under_transaction{false};
    Model* _model;
    Tag2Member tag2member;
    template <typename> friend class observable::scoped_on_change_t;
    template <typename,typename, typename> friend struct observable::value_impl;
    template <typename,typename> friend struct observable::unordered_set_impl;
    template <typename,typename, typename> friend struct observable::map_impl;
    template <typename,typename, typename> friend struct observable::memberclass_impl;
    template <typename,typename, typename> friend struct observable::variant_impl;
};

}
