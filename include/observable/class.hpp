
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "observable/is_observable.hpp"
#include "observable/traits.hpp"
#include "observable/types.hpp"
#include "observable/value.hpp"

#include <boost/fusion/include/at_key.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/map.hpp>
#include <boost/fusion/include/move.hpp>
#include <boost/fusion/include/pair.hpp>
#include <boost/signals2.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>

#include <array>
#include <type_traits>

namespace observable {

template<typename Derived>        
class class_ {
    using sig_t = boost::signals2::signal<void(const Derived&)>;
    sig_t _on_change;
public:    
    class_()
        : tag(boost::uuids::to_string
              (boost::uuids::random_generator{}()))
    {}
    
    template<typename F>
    boost::signals2::connection on_change(F&& f)
    { return _on_change.connect(std::forward<F>(f)); }

    const sig_t& on_change() const noexcept
    { return _on_change; }
    
    std::string tag;    
};
        
template<typename Derived>        
struct is_observable<class_<Derived>> : std::true_type {};
    
}
