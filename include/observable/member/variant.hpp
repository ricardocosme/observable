
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

namespace observable { namespace member {

struct variant_tag {};
    
template<typename Model_, typename Tag>
struct variant
{
    using model = Model_;
    using tag = Tag;
    using member_type = variant_tag;
};
        
template<typename Parent, typename Model>    
struct observable_of_int
{
    using type = observable::member::value_impl<Parent, Model>;
};

template<typename Parent, typename Model>
using observable_of_int_t = typename observable_of_int<Parent, Model>::type;
    
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
        using observable_t = observable_of_int_t<Parent,
                                                 typename std::decay<T>::type>;
        _ovariant = observable_t(_parent, o);
    }
    ObservableVariant& _ovariant;
    Parent& _parent;
};
    
template<typename Class, typename Model_, typename Enable = void>
struct variant_impl;
    
template<typename Class, typename Model_>
struct variant_impl<Class, Model_, void>
{
    using Model = Model_;
    using parent_t = Class;
    using types = typename Model::types;
    using Parent = variant_impl<Class, Model>;
    using observable_variant_t = typename boost::make_variant_over<
        typename boost::mpl::transform<
          types,
          observable_of_int<
              variant_impl<Class, Model>,
              boost::mpl::_1>
        >::type
    >::type;

    template<typename T>
    struct observable_of_impl
    {
        using type = value_impl<Parent, T>;
    };

    template<typename T>
    using observable_of = typename observable_of_impl<T>::type;
        
    variant_impl(parent_t& parent, Model& value)
        : _model(&value)
        , _parent(&parent)
    {}
    
    void set(Model o)
    {
        *_model = std::move(o);
        
        set_variant_t<observable_variant_t, Parent>
            set_variant(_ovariant, *this);
        
        boost::apply_visitor(set_variant, *_model);
        
        if (!_parent->_under_transaction)
        {
            _on_change(*_model);
            _parent->_on_change(*(_parent->_model));
        }
    }
    
    const Model& model() const noexcept
    { return *_model; }
    
    template<typename Visitor>
    void apply_visitor(Visitor&& visitor)
    { boost::apply_visitor(std::forward<Visitor>(visitor), _ovariant); }
    
    template<typename F>
    boost::signals2::connection on_change(F&& f)
    { return _on_change.connect(std::forward<F>(f)); }
    
    Model* _model{nullptr};
    parent_t* _parent{nullptr};
    
    observable_variant_t _ovariant;
    
    boost::signals2::signal<void(const Model&)> _on_change;
    
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
    
    }}
