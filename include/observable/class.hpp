#pragma once

#include "observable/class_member.hpp"
#include "observable/scoped_on_change.hpp"
#include <boost/signals2.hpp>

namespace observable {

template<typename T, typename M>
struct member
{
    using type = T;
    using name = M;
};
            
template<typename Model_, typename... Members>
struct class_ : class_member<class_<Model_, Members...>, Members...>
{
    using Model = Model_;
    using Base = class_member<class_<Model, Members...>, Members...>;
    
    Base& as_base() noexcept
    { return static_cast<Base&>(*this); }
    
    const Base& as_base() const noexcept
    { return static_cast<const Base&>(*this); }
    
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

    template<typename M>
    auto get() const noexcept -> decltype(as_base().get(M{}))
    { return as_base().get(M{}); }
    
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
    
    boost::signals2::signal<void(const Model&)> any_change;
private:    
    bool _under_transaction{false};
    Model& _model;
    template <typename, typename...> friend struct observable::class_member;
    template <typename> friend class observable::scoped_on_change_t;
};

}
