#pragma once

#include <boost/signals2.hpp>

namespace observable {
    
template<typename T>
struct get_type
{
    using type = typename T::type;
};
    
template<typename Class, typename... Members>    
struct class_member;

template<typename Class, typename Member, typename... Members>    
struct class_member<Class, Member, Members...> : class_member<Class, Members...>
{
    using Base = class_member<Class, Members...>;
    using Base::get;
    using Base::set;
    using Base::apply;
    using Base::on_change;
    
    using T = typename Member::type;
    using M = typename Member::name;

    template<typename... Ts>
    class_member(Class& parent, T& o, Ts&&... ts)
        : Base(parent, std::forward<Ts>(ts)...)
        , _parent(parent)
        , _value(o)
    {}

    const T& get(M) const noexcept
    { return _value; }
    
    void set(M, T o)
    {
        _value = std::move(o);
        if (!_parent._under_transaction)
        {
            _on_change(_value);
            _parent.any_change(_parent._model);
        }
    }

    template<typename F>
    void apply(M, F&& f)
    {
        f(_value);
        if (!_parent._under_transaction)
        {
            _on_change(_value);
            _parent.any_change(_parent._model);
        }        
    }

    template<typename F>
    boost::signals2::connection on_change(M, F&& f)
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
    using M = typename Member::name;
    
    class_member(Class& parent, T& o)
        : _parent(parent)
        , _value(o)
    {}

    const T& get(M) const noexcept
    { return _value; }
    
    void set(M, T o)
    {
        _value = std::move(o);
        if (!_parent._under_transaction)
        {
            _on_change(_value);
            _parent.any_change(_parent._model);
        }
    }

    template<typename F>
    void apply(M, F&& f)
    {
        f(_value);
        if (!_parent._under_transaction)
        {
            _on_change(_value);
            _parent.any_change(_parent._model);
        }        
    }

    template<typename F>
    boost::signals2::connection on_change(M, F&& f)
    {
        return _on_change.connect(std::forward<F>(f));
    }
    
    Class& _parent;
    T& _value;
    boost::signals2::signal<void(const T&)> _on_change;
};

}
