
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "observable/exp/traits.hpp"
#include "observable/exp/types.hpp"
#include "observable/detail/match_visitor.hpp"

#include <boost/signals2.hpp>
#include <boost/variant.hpp>

#include <type_traits>

namespace observable { namespace exp {

template<typename... T>
class variant
{
    boost::variant<T...> _variant;    
    boost::signals2::signal<void()> _on_change, _on_change_type;
public:
    using Model = boost::variant<T...>;
    
    variant() = default;

    template<typename U>
    variant(U o)
        : _variant(std::move(o))
    {}
    
    //?
    variant(const variant& rhs)
        : _variant(rhs._variant)
    {}

    //?
    variant& operator=(const variant& rhs)
    {
        auto before_type = _variant.which();
        _variant = rhs._variant;
        if(before_type != _variant.which())
            _on_change_type();
        _on_change();
        return *this;
    }

    const boost::variant<T...>& model() const noexcept
    { return _variant; }
    
    template<typename U>
    variant& operator=(U&& o)
    {
        auto before_type = _variant.which();
        _variant = std::forward<U>(o);
        if(before_type != _variant.which())
            _on_change_type();
        _on_change();
        return *this;
    }
        
    template<typename Visitor>
    typename std::decay<Visitor>::type::result_type
    visit(Visitor&& visitor)
    { return boost::apply_visitor(std::forward<Visitor>(visitor), _variant); }

    template<typename Visitor>
    typename std::decay<Visitor>::type::result_type
    visit(Visitor&& visitor) const
    { return boost::apply_visitor(std::forward<Visitor>(visitor), _variant); }
    
    template<typename... Fs>
    inline void match(Fs&&... fs)
    {
        auto visitor = detail::match_visitor<typename std::decay<Fs>::type...>
            (std::forward<Fs>(fs)...);
        visit(std::move(visitor));
    }
    
    template<typename F>
    boost::signals2::connection on_change(F&& f)
    { return _on_change.connect(std::forward<F>(f)); }
    
    template<typename F>
    boost::signals2::connection on_change_type(F&& f)
    { return _on_change_type.connect(std::forward<F>(f)); }
};

}}
