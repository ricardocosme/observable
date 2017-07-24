#pragma once

#include "person.hpp"

#include "observable/class.hpp"

#include <cstddef>
#include <string>

struct name{};
struct age{};
struct skills{};

using observable_person = observable::class_<
    person_t,
    observable::member<std::string, name>,
    observable::member<std::size_t, age>,
    observable::member<skills_t,    skills>
    >;
