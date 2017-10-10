
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "observable/is_observable.hpp"
#include "observable/detail/match_visitor.hpp"

#include <boost/signals2.hpp>
#include <boost/variant.hpp>

#include <type_traits>

namespace observable {

namespace serialization {
    struct variant;
}

template<typename... T>
class variant
{
    boost::variant<T...> _variant;    
    boost::signals2::signal<void(const variant<T...>&)> _on_change, _on_change_type;

    friend serialization::variant;
public:
    using Model = boost::variant<T...>;
    
    variant() = default;

    template<typename U>
    variant(U o)
        : _variant(std::move(o))
    {}
    
    // ?
    variant(const variant& rhs)
        : _variant(rhs._variant)
    {}

    // ?
    variant& operator=(const variant& rhs)
    {
        auto before_type = _variant.which();
        _variant = rhs._variant;
        if(before_type != _variant.which())
            _on_change_type(*this);
        _on_change(*this);
        return *this;
    }

    variant(variant&&) = default;
    variant& operator=(variant&&) = default;

    const boost::variant<T...>& model() const noexcept
    { return _variant; }
    
    template<typename U>
    variant& operator=(U&& o)
    {
        auto before_type = _variant.which();
        _variant = std::forward<U>(o);
        if(before_type != _variant.which())
            _on_change_type(*this);
        _on_change(*this);
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

template<typename... T>        
struct is_observable<variant<T...>> : std::true_type {};

}
