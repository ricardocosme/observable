
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "observable/traits.hpp"
#include "observable/member/value.hpp"
#include "observable/member/unordered_set.hpp"
#include "observable/member/map.hpp"
#include "observable/member/variant.hpp"
#include "observable/member/vector.hpp"
#include <boost/signals2.hpp>
#include <boost/fusion/include/map.hpp>
#include <boost/fusion/include/at_key.hpp>
#include <boost/fusion/include/make_map.hpp>
#include <boost/fusion/include/pair.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/move.hpp>
#include <boost/fusion/sequence/intrinsic/at_key.hpp>
#include <type_traits>

namespace observable {

template<typename>
class scoped_on_change_t;
                    
template<typename Parent>    
struct set_on_change
{
    set_on_change(Parent& p) : _parent(p) {}
    
    template<typename T>
    void operator()(T& o) const
    {
        auto& parent = _parent;
        parent.sc.push_back(
            o.second.on_change(
            [&parent](const typename T::second_type::Model&)
            { parent._on_change(parent.model()); })
            );
    }
    
    Parent& _parent;
};
    
template<typename Model>
struct observable_of {
    using type = typename std::conditional<
        is_vector<Model>::value,
        typename member::vector_impl<Model>,
        typename std::conditional<
            is_class<Model>::value,
            typename is_class<Model>::type,
            typename std::conditional<
                is_variant<Model>::value,
                member::variant_impl<Model>,
                typename std::conditional<
                    is_unordered_set<Model>::value,
                    member::unordered_set_impl<Model>,
                    typename std::conditional<
                        is_map<Model>::value,
                        member::map_impl<Model>,
                        typename member::value_impl<Model>
                    >::type
                >::type
            >::type
        >::type
    >::type;
};
    
template<typename Model_, typename... Members>
class class_ 
{
public:
    using Model = Model_;
    using Tag2Member = boost::fusion::map<
        boost::fusion::pair<
            typename Members::second_type,
            observable_of_t<typename Members::first_type>
        >...>;

    class_() = default;
    
    class_(Model& model,
           typename Members::first_type&... args)
        : _model(&model)
        , _tag2member
          (boost::fusion::pair<
           typename Members::second_type,
           observable_of_t<typename Members::first_type>>
           (observable_of_t<typename Members::first_type>
            (factory(args)))...)
    {
        for_each(_tag2member,
                 set_on_change<class_<Model_, Members...>>{*this});
    }

    class_(class_&& rhs) noexcept
        : _model(rhs._model)
        , _on_change(std::move(rhs._on_change))
        , _under_transaction(rhs._under_transaction)
    {
        rhs.sc.clear();
        for_each(rhs._tag2member,
                 set_on_change<class_<Model_, Members...>>{*this});
        boost::fusion::move(std::move(rhs._tag2member), _tag2member);        
    }
    
    class_& operator=(class_&& rhs) noexcept
    {
        _model = rhs._model;
        rhs.sc.clear();
        for_each(rhs._tag2member,
                 set_on_change<class_<Model_, Members...>>{*this});
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
    std::vector<boost::signals2::scoped_connection> sc;
    template <typename>
    friend class observable::scoped_on_change_t;
    template <typename>
    friend struct observable::member::map_impl;
    template <typename>
    friend struct observable::member::value_impl;
    template <typename>
    friend struct observable::member::variant_impl;
    template <typename>
    friend struct observable::member::vector_impl;
    template <typename>
    friend struct observable::member::unordered_set_impl;
    template<typename>
    friend struct set_on_change;
};

}
