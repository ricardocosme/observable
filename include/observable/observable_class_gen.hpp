// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <boost/preprocessor.hpp>

#define OBSERVABLE_observed(p) \
    BOOST_PP_TUPLE_ELEM(3, 0, p)

#define OBSERVABLE_type(p) \
    BOOST_PP_TUPLE_ELEM(3, 1, p)

#define OBSERVABLE_tag(p) \
    BOOST_PP_TUPLE_ELEM(3, 2, p)

#define OBSERVABLE_gen_member(z, idx, members) \
    observable::member< \
        OBSERVABLE_observed(BOOST_PP_SEQ_ELEM(idx, members)), \
                        OBSERVABLE_type(BOOST_PP_SEQ_ELEM(idx, members)), \
                        &OBSERVABLE_observed(BOOST_PP_SEQ_ELEM(idx, members))::OBSERVABLE_tag(BOOST_PP_SEQ_ELEM(idx, members))>


#define OBSERVABLE_gen_members(observed, members) \
    BOOST_PP_ENUM(BOOST_PP_SEQ_SIZE(members), OBSERVABLE_gen_member, members)
    
#define OBSERVABLE_gen_oclass(oclass, observed, members) \
    ::observable::class_< \
        oclass, observed, OBSERVABLE_gen_members(observed, members) \
    >

#define OBSERVABLE_CLASS(oclass, observed, members)      \
    OBSERVABLE_gen_oclass(oclass, observed, members) 
