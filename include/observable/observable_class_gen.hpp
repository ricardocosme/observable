
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <boost/preprocessor.hpp>

#define OBSERVABLE_type(p) \
    BOOST_PP_TUPLE_ELEM(2, 0, p)

#define OBSERVABLE_tag(p) \
    BOOST_PP_TUPLE_ELEM(2, 1, p)

#define OBSERVABLE_gen_member(z, idx, members) \
    std::pair< \
        OBSERVABLE_type(BOOST_PP_SEQ_ELEM(idx, members)), \
        OBSERVABLE_tag(BOOST_PP_SEQ_ELEM(idx, members))>

#define OBSERVABLE_gen_members(members) \
    BOOST_PP_ENUM(BOOST_PP_SEQ_SIZE(members), OBSERVABLE_gen_member, members)
    
#define OBSERVABLE_gen_tag(z, idx, members) \
    struct OBSERVABLE_tag(BOOST_PP_SEQ_ELEM(idx, members)) {};

#define OBSERVABLE_gen_tags(members) \
    BOOST_PP_REPEAT(BOOST_PP_SEQ_SIZE(members), OBSERVABLE_gen_tag, members)

#define OBSERVABLE_gen_oclass(observed, members) \
    ::observable::class_<                      \
        observed, OBSERVABLE_gen_members(members) \
    >

#define OBSERVABLE_gen_factory_param(z, idx, members) \
    m.OBSERVABLE_tag(BOOST_PP_SEQ_ELEM(idx, members))

#define OBSERVABLE_gen_factory_params(members) \
    BOOST_PP_ENUM(BOOST_PP_SEQ_SIZE(members), OBSERVABLE_gen_factory_param, members)

#define OBSERVABLE_CLASS_GEN(oclass, observed, members) \
    OBSERVABLE_gen_tags(members) \
    using oclass = OBSERVABLE_gen_oclass(observed, members);
