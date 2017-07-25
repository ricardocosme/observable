
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <boost/signals2.hpp>

namespace observable {
    
template<typename Class, typename... Members>    
struct class_member;

template<typename Class, typename Member, typename... Members>    
struct class_member<Class, Member, Members...> : class_member<Class, Members...>
{
    using Base = class_member<Class, Members...>;
    using Base::apply;
    using Base::get;
    using Base::on_change;
    using Base::set;
    
    using T = typename Member::type;
    using Tag = typename Member::tag;

    template<typename... Ts>
    class_member(Class& parent, T& o, Ts&... ts)
        : Base(parent, ts...)
        , _parent(parent)
        , _value(o)
    {}

    const T& get(Tag) const noexcept
    { return _value; }
    
    void set(Tag, T o)
    {
        _value = std::move(o);
        if (!_parent._under_transaction)
        {
            _on_change(_value);
            _parent.any_change(_parent._model);
        }
    }

    template<typename F>
    void apply(Tag, F&& f)
    {
        f(_value);
        if (!_parent._under_transaction)
        {
            _on_change(_value);
            _parent.any_change(_parent._model);
        }        
    }

    template<typename F>
    boost::signals2::connection on_change(Tag, F&& f)
    {
        return _on_change.connect(std::forward<F>(f));
    }
    
    Class& _parent;
    T& _value;
    boost::signals2::signal<void(T)> _on_change;
};

template<typename Class, typename Member>
struct class_member<Class, Member>
{
    using T = typename Member::type;
    using Tag = typename Member::tag;
    
    class_member(Class& parent, T& o)
        : _parent(parent)
        , _value(o)
    {}

    const T& get(Tag) const noexcept
    { return _value; }
    
    void set(Tag, T o)
    {
        _value = std::move(o);
        if (!_parent._under_transaction)
        {
            _on_change(_value);
            _parent.any_change(_parent._model);
        }
    }

    template<typename F>
    void apply(Tag, F&& f)
    {
        f(_value);
        if (!_parent._under_transaction)
        {
            _on_change(_value);
            _parent.any_change(_parent._model);
        }        
    }

    template<typename F>
    boost::signals2::connection on_change(Tag, F&& f)
    {
        return _on_change.connect(std::forward<F>(f));
    }
    
    Class& _parent;
    T& _value;
    boost::signals2::signal<void(const T&)> _on_change;
};

}
