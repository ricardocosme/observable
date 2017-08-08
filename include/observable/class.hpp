
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "observable/traits.hpp"
#include "observable/value.hpp"
#include "observable/unordered_set.hpp"
#include "observable/map.hpp"
#include "observable/variant.hpp"
#include "observable/vector.hpp"
#include <boost/signals2.hpp>
#include <boost/fusion/include/map.hpp>
#include <boost/fusion/include/at_key.hpp>
#include <boost/fusion/include/make_map.hpp>
#include <boost/fusion/include/pair.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/move.hpp>
#include <boost/fusion/sequence/intrinsic/at_key.hpp>
#include <array>
#include <type_traits>

namespace observable {

template<typename Parent>    
struct set_on_change
{
    set_on_change(Parent& p)
        : _parent(p)
        , _it(_parent.observable_on_change_conns.begin())
    {}
    
    template<typename T>
    void operator()(T& o) const
    {
        auto& parent = _parent;
        *_it++ = 
            o.second.on_change(
            [&parent](const typename T::second_type::Model&)
            { parent._on_change(parent.get()); });
    }
    
    Parent& _parent;
    mutable typename Parent::observable_on_change_conns_t::iterator _it;
};
    
template<typename Model>
struct observable_of {
    using type = typename std::conditional<
        is_vector<Model>::value,
        vector<Model>,
        typename std::conditional<
            is_class<Model>::value,
            typename is_class<Model>::type,
            typename std::conditional<
                is_variant<Model>::value,
                variant<Model>,
                typename std::conditional<
                    is_unordered_set<Model>::value,
                    unordered_set<Model>,
                    typename std::conditional<
                        is_map<Model>::value,
                        map<Model>,
                        value<Model>
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
    using Tag2Observable = boost::fusion::map<
        boost::fusion::pair<
            typename Members::second_type,
            observable_of_t<typename Members::first_type>
        >...>;

    class_() = default;
    
    class_(Model& model,
           typename Members::first_type&... args)
        : _model(&model)
        , _tag2observable
          (boost::fusion::pair<
           typename Members::second_type,
           observable_of_t<typename Members::first_type>>
           (observable_of_t<typename Members::first_type>
            (observable_factory(args)))...)
    {
        set_on_change<class_<Model_, Members...>> visitor{*this};
        for_each(_tag2observable, visitor);
    }

    class_(class_&& rhs) noexcept
        : _model(rhs._model)
        , _on_change(std::move(rhs._on_change))
    {
        rhs.observable_on_change_conns.swap(observable_on_change_conns);
        set_on_change<class_<Model_, Members...>> visitor{*this};
        for_each(rhs._tag2observable, visitor);
        boost::fusion::move(std::move(rhs._tag2observable), _tag2observable);        
    }
    
    class_& operator=(class_&& rhs) noexcept
    {
        _model = rhs._model;
        rhs.observable_on_change_conns.swap(observable_on_change_conns);        
        set_on_change<class_<Model_, Members...>> visitor{*this};
        for_each(rhs._tag2observable, visitor);
        boost::fusion::move(std::move(rhs._tag2observable), _tag2observable);
        _on_change = std::move(rhs._on_change);
        return *this;
    }
    
    template<typename Tag, typename T>
    void assign(T o)
    { return boost::fusion::at_key<Tag>(_tag2observable).assign(std::move(o)); }

    template<typename Tag>
    auto get() ->
        decltype(boost::fusion::at_key<Tag>(
                     std::declval<
                     typename std::add_lvalue_reference<Tag2Observable>::type>()))
    { return boost::fusion::at_key<Tag>(_tag2observable); }
    
    template<typename Tag, typename F>
    boost::signals2::connection on_change(F&& f)
    {
        return boost::fusion::at_key<Tag>
            (_tag2observable).on_change(std::forward<F>(f));
    }
    
    template<typename F>
    boost::signals2::connection on_change(F&& f)
    { return _on_change.connect(std::forward<F>(f)); }
    
    const Model& get() const noexcept
    { return *_model; }

private:
    
    Model* _model{nullptr};
    Tag2Observable _tag2observable;
    boost::signals2::signal<void(const Model&)> _on_change;

    using observable_on_change_conns_t =
        std::array<boost::signals2::scoped_connection, sizeof...(Members)>;
    
    observable_on_change_conns_t observable_on_change_conns;
    
    template <typename>
    friend struct observable::map;
    
    template <typename>
    friend struct observable::value;
    
    template <typename>
    friend struct observable::variant;
    
    template <typename>
    friend struct observable::vector;
    
    template <typename>
    friend struct observable::unordered_set;
    
    template<typename>
    friend struct set_on_change;
};

}
