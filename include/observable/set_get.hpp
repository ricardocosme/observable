
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <boost/signals2.hpp>

namespace observable {
    
template<typename Observed_>
struct set_get
{
    using Observed = Observed_;
    using Setter = std::function<void(Observed)>;
    using Getter = std::function<const Observed&()>;
    Setter setter;
    Getter getter;
};

}
