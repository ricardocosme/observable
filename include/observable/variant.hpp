
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "observable/traits.hpp"
#include "observable/types.hpp"

#include <boost/signals2.hpp>
#include <boost/variant.hpp>

namespace observable { 

template<typename Observed_>
struct variant;
    
template<typename Observed>
struct observable_of<
    Observed,
    typename std::enable_if<is_variant<Observed>::value>::type
>
{
    using type = variant<Observed>;
};
    
template<typename ObservableVariant>
struct set_variant_t
{
    using result_type = void;

    set_variant_t(ObservableVariant& ovariant)
        : _ovariant(ovariant)
    {}
    
    template<typename T>
    result_type operator()(T& o) const
    {    
        using observable_t = observable_of_t
            <typename std::decay<T>::type>;
        _ovariant = observable_t(observable_factory(o));
    }
    ObservableVariant& _ovariant;
};
    
template<typename Observed_>
struct variant
{
    using Observed = Observed_;
    using types = typename Observed::types;
    using observable_variant_t = typename boost::make_variant_over<
        typename boost::mpl::transform<
          types,
          observable_of<boost::mpl::_1>
        >::type
    >::type;

    variant() = default;
    
    variant(Observed& value)
        : _observed(&value)
    {}

    template<typename T>
    variant& operator=(T&& o)
    {
        assign(std::forward<T>(o));
        return *this;
    }
    
    void assign(Observed o)
    {
        *_observed = std::move(o);
        
        set_variant_t<observable_variant_t> set_variant(_ovariant);
        
        boost::apply_visitor(set_variant, *_observed);
        
        _on_change(*_observed);
    }
    
    const Observed& get() const noexcept
    { return *_observed; }
    
    template<typename Visitor>
    void apply_visitor(Visitor&& visitor)
    { boost::apply_visitor(std::forward<Visitor>(visitor), _ovariant); }
    
    template<typename F>
    boost::signals2::connection on_change(F&& f)
    { return _on_change.connect(std::forward<F>(f)); }
    
    Observed* _observed{nullptr};
    
    observable_variant_t _ovariant;
    
    boost::signals2::signal<void(const Observed&)> _on_change;
};
    
}
