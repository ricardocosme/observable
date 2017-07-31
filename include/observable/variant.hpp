
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "observable/types.hpp"

#include <boost/signals2.hpp>
#include <boost/variant.hpp>
#include <boost/fusion/container/generation/make_map.hpp>
#include <boost/mpl/map.hpp>
#include <boost/mpl/insert.hpp>
#include <boost/mpl/iter_fold.hpp>
#include <boost/mpl/fold.hpp>

namespace observable {

struct variant_member_tag {};
    
template<typename T, typename Tag>
struct variant
{
    using tag = Tag;
    using type = T;
    using member_type = variant_member_tag;
};
        
template<typename Parent, typename T>    
struct observable_of
{
    using type = observable::value_impl<Parent, T>;
};

template<typename Parent, typename T>
using observable_of_t = typename observable_of<Parent, T>::type;
    
template<typename ObservableVariant, typename Parent>
struct set_variant_t
{
    using result_type = void;

    set_variant_t(ObservableVariant& ovariant, Parent& parent)
        : _ovariant(ovariant)
        , _parent(parent)
    {}
    
    template<typename T>
    result_type operator()(T& o) const
    {    
        using decayed_t = typename std::decay<T>::type;
        using observable_t = typename observable_of<Parent, decayed_t>::type;
        _ovariant = observable_t(_parent, o);
    }
    ObservableVariant& _ovariant;
    Parent& _parent;
};
    
template<typename Class, typename Model, typename Enable = void>
struct variant_impl;
    
template<typename Class, typename Model>
struct variant_impl<Class, Model, void>
{
    using type = Model;
    using parent_t = Class;
    using types = typename Model::types;
    using Parent = variant_impl<Class, Model>;
    using observable_variant_t = typename boost::make_variant_over<
        typename boost::mpl::transform<
          types,
          observable_of<
              variant_impl<Class, Model>,
              boost::mpl::_1>
        >::type
    >::type;
    using tags = typename boost::mpl::fold<
        typename observable_variant_t::types,
        boost::mpl::map<>,
        boost::mpl::insert<boost::mpl::_1,
                           boost::mpl::pair<boost::mpl::_1, boost::mpl::_2>>>::type;
        
    variant_impl(parent_t& parent, type& value)
        : _model(&value)
        , _parent(&parent)
    {}
    
    // template<typename T>
    // static constexpr T observable_of()
    // {
    //     return ::observable::observable_of<Parent, T>::type;
    // }
    
    void set(type o)
    {
        *_model = std::move(o);
        if (!_parent->_under_transaction)
        {
            _on_change(*_model);
            _parent->_on_change(*(_parent->_model));
        }
    }
    const type& get() const noexcept
    { return *_model; }
    
    template<typename Visitor>
    void apply_visitor(Visitor&& visitor)
    {
        set_variant_t<observable_variant_t, Parent> set_variant(_ovariant, *this);
        boost::apply_visitor(set_variant, *_model);
        boost::apply_visitor(std::forward<Visitor>(visitor), _ovariant);
    }
    
    template<typename F>
    boost::signals2::connection on_change(F&& f)
    {
        return _on_change.connect(std::forward<F>(f));
    }
    type* _model;
    parent_t* _parent;
    observable_variant_t _ovariant;
    boost::signals2::signal<void(const type&)> _on_change;
    bool _under_transaction{false};
};
    
// template<typename Class, typename Model>
// struct variant_impl<Class, Model, container_tag>
// {
//     using type = Model;
//     using parent_t = Class;
//     variant_impl(parent_t& parent, type& value,
//                typename Class::type::const_iterator it)
//         : _model(&value)
//         , _parent(&parent)
//         , _on_change(std::make_shared<boost::signals2::signal<void(const type&)>>())
//         , _it(it)
//     {}
    
//     void set(type o)
//     {
//         *_model = std::move(o);
//         if (!_parent->_parent->_under_transaction)
//         {
//             (*_on_change)(*_model);
//             _parent->_on_model_change(_parent->_model, _it);
//         }
//     }
    
//     const type& get() const noexcept
//     { return *_model; }    
    
//     template<typename Visitor>
//     void apply_visitor(Visitor&& visitor)
//     {
//         boost::apply_visitor(std::forward<Visitor>(visitor), *_model);
//     }
    
//     template<typename F>
//     boost::signals2::connection on_change(F&& f)
//     {
//         return _on_change->connect(std::forward<F>(f));
//     }
//     type* _model;
//     parent_t* _parent;
//     std::shared_ptr<boost::signals2::signal<void(const type&)>> _on_change;
//     typename Class::type::const_iterator _it;
// };
    
}
