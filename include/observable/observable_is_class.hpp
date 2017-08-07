
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#define OBSERVABLE_IS_CLASS(oclass) \
namespace observable { \
    template<> \
    struct is_class<typename oclass::Model> \
        : std::true_type \
    { using type = oclass; }; \
}
