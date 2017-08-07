// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <boost/signals2.hpp>
#include <boost/fusion/include/at_key.hpp>
#include <type_traits>

namespace observable { namespace member {
    
struct class_tag {};

template<typename Model, typename ObservableClass, typename Tag>
struct class_
{
    using member_type = class_tag;
    using tag = Tag;
    using model = Model;
    using observable_class = ObservableClass;
};

template<typename Class, typename Model_, typename ObservableClass, typename Enable = void>
struct class_impl;
    
template<typename Class, typename Model_, typename ObservableClass>
struct class_impl<Class, Model_, ObservableClass, void>
{
    using Model = Model_;
    using parent_t = Class;

    class_impl() = default;
    
    class_impl(parent_t& parent, Model& model)
        : _observable_class(factory(model))
        , _parent(&parent)
    {
        _parent_on_change_conn = _observable_class.on_change
            ([&parent](const typename ObservableClass::Model&)
             { parent._on_change(parent.model()); });
    }

    class_impl(class_impl&& rhs) noexcept
        : _observable_class(std::move(rhs._observable_class))
        , _parent(rhs._parent)
    {
        auto& parent = *_parent;
        _parent_on_change_conn = _observable_class.on_change
            ([&parent](const typename ObservableClass::Model&)
             { parent._on_change(parent.model()); });
    }
    
    class_impl& operator=(class_impl&& rhs) noexcept
    {
        _observable_class = std::move(rhs._observable_class);
        _parent = rhs._parent;
        auto& parent = *_parent;
        _parent_on_change_conn = _observable_class.on_change
            ([&parent](const typename ObservableClass::Model&)
             { parent._on_change(parent.model()); });
        return *this;
    }
    
    template<typename M, typename V>
    void set(V o)
    {
        return _observable_class.template set<M>(std::move(o));
    }
    
    template<typename M>
    auto get() ->
        decltype(boost::fusion::at_key<M>(
                     std::declval<
                     typename std::add_lvalue_reference<
                     typename ObservableClass::Tag2Member>::type>()))
    { return boost::fusion::at_key<M>(_observable_class._tag2member); }
    
    template<typename F>
    boost::signals2::connection on_change(F&& f)
    { return _observable_class._on_change.connect(std::forward<F>(f)); }

    ObservableClass _observable_class;
    parent_t* _parent{nullptr};
    boost::signals2::scoped_connection _parent_on_change_conn;
};
    
template<typename Class, typename Model_, typename ObservableClass>
struct class_impl<Class, Model_, ObservableClass, container_tag>
{
    using Model = Model_;
    using parent_t = Class;

    class_impl() = default;
    
    class_impl(parent_t& parent, Model& model,
               typename Class::Model::const_iterator it)
        : _observable_class(factory(model))
        , _parent(&parent)
        , _it(it)
    {
        //TODO: ref2on_change. See value.hpp
        _parent_on_change_conn = _observable_class.on_change
            ([&parent](const typename ObservableClass::Model&)
             { parent._on_change(parent.model()); });
        
        _parent_on_value_change_conn = _observable_class.on_change
            ([&parent, it](const typename ObservableClass::Model&)
             { parent._on_value_change(parent.model(), it); });
    }

    class_impl(class_impl&& rhs)
        : _observable_class(std::move(rhs._observable_class))
        , _parent(rhs._parent)
    {
        auto& parent = *_parent;
        _parent_on_change_conn = _observable_class.on_change
            ([&parent](const typename ObservableClass::Model&)
             { parent._on_change(parent.model()); });

        auto it = _it;
        _parent_on_value_change_conn = _observable_class.on_change
            ([&parent, it](const typename ObservableClass::Model&)
             { parent._on_value_change(parent.model(), it); });
    }
    
    class_impl& operator=(class_impl&& rhs)
    {
        _observable_class = std::move(rhs._observable_class);
        _parent = rhs._parent;
        auto& parent = *_parent;
        _parent_on_change_conn = _observable_class.on_change
            ([&parent](const typename ObservableClass::Model&)
             { parent._on_change(parent.model()); });

        auto it = _it;
        _parent_on_value_change_conn = _observable_class.on_change
            ([&parent, it](const typename ObservableClass::Model&)
             { parent._on_value_change(parent.model(), it); });
        return *this;
    }
    
    template<typename M, typename V>
    void set(V o)
    { return _observable_class.template set<M>(std::move(o)); }
    
    template<typename M>
    auto get() ->
        decltype(boost::fusion::at_key<M>(
                     std::declval<
                     typename std::add_lvalue_reference<
                     typename ObservableClass::Tag2Member>::type>()))
    { return boost::fusion::at_key<M>(_observable_class._tag2member); }
    
    template<typename M, typename F>
    boost::signals2::connection on_change(F&& f)
    {
        return boost::fusion::at_key<M>
            (_observable_class._tag2member).on_change(std::forward<F>(f));
    }
    
    template<typename F>
    boost::signals2::connection on_change(F&& f)
    { return _observable_class._on_change.connect(std::forward<F>(f)); }
    
    ObservableClass _observable_class;
    parent_t* _parent{nullptr};
    
    boost::signals2::scoped_connection _parent_on_change_conn,
        _parent_on_value_change_conn;
    
    typename Class::Model::const_iterator _it;
};
        
}}
    
