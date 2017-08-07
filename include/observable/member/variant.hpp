
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
        _ovariant = observable_t(factory(o));
    }
    ObservableVariant& _ovariant;
};
    
template<typename Model_>
struct variant_impl
{
    using Model = Model_;
    using types = typename Model::types;
    using observable_variant_t = typename boost::make_variant_over<
        typename boost::mpl::transform<
          types,
          observable_of<boost::mpl::_1>
        >::type
    >::type;

    variant_impl(Model& value)
        : _model(&value)
    {}

    template<typename T>
    variant_impl& operator=(T&& o)
    {
        set(std::forward<T>(o));
        return *this;
    }
    
    void set(Model o)
    {
        *_model = std::move(o);
        
        set_variant_t<observable_variant_t> set_variant(_ovariant);
        
        boost::apply_visitor(set_variant, *_model);
        
        _on_change(*_model);
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
    
    observable_variant_t _ovariant;
    
    boost::signals2::signal<void(const Model&)> _on_change;
    
    bool _under_transaction{false};
};
    
    }}
