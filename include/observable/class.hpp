#pragma once

#include "observable/scoped_on_change.hpp"
#include <boost/signals2.hpp>

namespace observable {

template<typename T, typename M>
struct member
{
    using type = T;
    using name = M;
};
    
template<typename T>
struct get_type
{
    using type = typename T::type;
};
    
template<typename Class, typename... Members>    
struct class_value;

template<typename Class, typename Member, typename... Members>    
struct class_value<Class, Member, Members...> : class_value<Class, Members...>
{
    using Base = class_value<Class, Members...>;
    using Base::set;
    using Base::apply;
    using Base::on_change;
    
    using T = typename Member::type;
    using M = typename Member::name;

    template<typename... Ts>
    class_value(Class& parent, T& o, Ts&&... ts)
        : Base(parent, std::forward<Ts>(ts)...)
        , _parent(parent)
        , _value(o)
    {}

    void set(M, T o)
    {
        _value = std::move(o);
        if (!_parent._under_transaction)
        {
            _on_change(_value);
            _parent.any_change();
        }
    }

    const T& get() const noexcept
    { return _value; }
    
    template<typename F>
    void apply(M, F&& f)
    {
        f(_value);
        if (!_parent._under_transaction)
        {
            _on_change(_value);
            _parent.any_change();
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
struct class_value<Class, Member>
{
    using T = typename Member::type;
    using M = typename Member::name;
    
    class_value(Class& parent, T& o)
        : _parent(parent)
        , _value(o)
    {}

    void set(M, T o)
    {
        _value = std::move(o);
        if (!_parent._under_transaction)
        {
            _on_change(_value);
            _parent.any_change();
        }
    }

    const T& get() const noexcept
    { return _value; }
    
    template<typename F>
    void apply(M, F&& f)
    {
        f(_value);
        if (!_parent._under_transaction)
        {
            _on_change(_value);
            _parent.any_change();
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
        
template<typename Model, typename... Members>
struct class_ : class_value<class_<Model, Members...>, Members...>
{
    using Base = class_value<class_<Model, Members...>, Members...>;
    
    Base& as_base() noexcept
    { return static_cast<Base&>(*this); }
    class_(Model& model,
           typename get_type<Members>::type&... args)
        : Base(*this, args...)
        , _model(model)
    {}

    template<typename M, typename V>
    void set(V o)
    {
        as_base().set(M{}, std::move(o));
    }

    template<typename M, typename F>
    void apply(F&& f)
    {
        as_base().apply(M{}, std::forward<F>(f));
    }
    
    template<typename M, typename F>
    boost::signals2::connection on_change(F&& f)
    {
        return as_base().on_change(M{}, std::forward<F>(f));
    }
    
    template<typename F>
    boost::signals2::connection on_change(F&& f)
    {
        return any_change.connect(std::forward<F>(f));
    }
    
    const Model& model() const noexcept
    { return _model; }
    
    boost::signals2::signal<void()> any_change;    
// private:    
    bool _under_transaction{false};
    Model& _model;
private:    
    template <typename> friend class observable::scoped_on_change_t;
};

}
