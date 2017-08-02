
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "observable/member/value.hpp"
#include "observable/member/class.hpp"
#include "observable/member/unordered_set.hpp"
#include "observable/member/map.hpp"
#include "observable/member/vector.hpp"
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
struct observable_impl;

template<typename Class, typename Member>
struct observable_impl<
    Class,
    Member,
    typename std::enable_if<
        std::is_same<
            typename Member::member_type,
            member::value_tag
        >::value
    >::type
>
{
    using type = member::value_impl<Class, typename Member::model>;
};
        
template<typename Class, typename Member>
struct observable_impl<
    Class,
    Member,
    typename std::enable_if<
        std::is_same<
            typename Member::member_type,
            member::unordered_set_tag
        >::value
    >::type
>
{
    using type = member::unordered_set_impl<Class, typename Member::model>;
};
    
template<typename Class, typename Member>
struct observable_impl<
    Class,
    Member,
    typename std::enable_if<
        std::is_same<
            typename Member::member_type,
            member::map_tag
        >::value
    >::type
>
{
    using type = member::map_impl<Class,
                          typename Member::model,
                          typename Member::observable_value>;
};
        
template<typename Class, typename Member>
struct observable_impl<
    Class,
    Member,
    typename std::enable_if<
        std::is_same<
            typename Member::member_type,
            member::class_tag
        >::value
    >::type
>
{
    using type = member::class_impl<Class,
                                    typename Member::model,
                                    typename Member::observable_class>;
};

template<typename Class, typename Member>
struct observable_impl<
    Class,
    Member,
    typename std::enable_if<
        std::is_same<
            typename Member::member_type,
            member::variant_tag
        >::value
    >::type
>
{
    using type = member::variant_impl<Class, typename Member::model>;
};
        
template<typename Class, typename Member>
struct observable_impl<
    Class,
    Member,
    typename std::enable_if<
        std::is_same<
            typename Member::member_type,
            member::vector_tag
        >::value
    >::type
>
{
    using type = member::vector_impl<Class,
                                     typename Member::model,
                                     typename Member::observable_value_type>;
};
    
template<typename Parent>    
struct set_parent
{
    set_parent(Parent& p) : _parent(p) {}
    
    template<typename T>
    void operator()(T& o) const
    { o.second._parent = &_parent; }
    
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
            typename observable_impl<class_<Model, Members...>, Members>::type
        >...>;

    class_() = default;
    
    class_(Model& model,
           typename Members::model&... args)
        : _model(&model)
        , _tag2member
          (boost::fusion::pair<
           typename Members::tag,
           typename observable_impl<class_<Model, Members...>, Members>::type>
           (typename observable_impl<class_<Model, Members...>, Members>::type
            (*this, args))...)
    {}

    class_(class_&& rhs)
        : _model(rhs._model)
        , _on_change(std::move(rhs._on_change))
        , _under_transaction(rhs._under_transaction)
    {
        for_each(rhs._tag2member,
                 set_parent<class_<Model_, Members...>>{*this});
        boost::fusion::move(std::move(rhs._tag2member), _tag2member);
    }
    
    class_& operator=(class_&& rhs)
    {
        _model = rhs._model;
        for_each(rhs._tag2member,
                 set_parent<class_<Model_, Members...>>{*this});
        boost::fusion::move(std::move(rhs._tag2member), _tag2member);
        _on_change = std::move(rhs._on_change);
        _under_transaction = rhs._under_transaction;
        return *this;
    }
    
    template<typename M, typename V>
    void set(V o)
    { return boost::fusion::at_key<M>(_tag2member).set(std::move(o)); }

    template<typename M>
    auto get() ->
        decltype(boost::fusion::at_key<M>(
                     std::declval<
                     typename std::add_lvalue_reference<Tag2Member>::type>()))
    { return boost::fusion::at_key<M>(_tag2member); }
    
    template<typename M, typename F>
    boost::signals2::connection on_change(F&& f)
    {
        return boost::fusion::at_key<M>
            (_tag2member).on_change(std::forward<F>(f));
    }
    
    template<typename F>
    boost::signals2::connection on_change(F&& f)
    { return _on_change.connect(std::forward<F>(f)); }
    
    const Model& model() const noexcept
    { return *_model; }
    
private:
    
    Model* _model{nullptr};
    Tag2Member _tag2member;
    boost::signals2::signal<void(const Model&)> _on_change;
    bool _under_transaction{false};
    template <typename> friend class observable::scoped_on_change_t;
    template <typename,typename, typename, typename>
    friend struct observable::member::class_impl;
    template <typename,typename, typename>
    friend struct observable::member::map_impl;
    template <typename,typename, typename>
    friend struct observable::member::value_impl;
    template <typename,typename, typename>
    friend struct observable::member::variant_impl;
    template <typename,typename, typename>
    friend struct observable::member::vector_impl;
    template <typename,typename>
    friend struct observable::member::unordered_set_impl;
};

}
