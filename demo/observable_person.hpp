#pragma once

#include "person.hpp"

#include "observable/class.hpp"
#include "observable/unordered_set.hpp"

#include <cstddef>
#include <string>

struct name{};
struct age{};
struct skills{};
struct kids{};

using observable_person = observable::class_<
    person_t,
    observable::value<std::string, name>,
    observable::value<std::size_t, age>,
    observable::value<skills_t,    skills>,
    observable::unordered_set<kids_t, kids>
    >;

namespace observable {

inline observable_person factory(person_t& model)
{
    return observable_person(model, model.name, model.age, model.skills, model.kids);
}
    
}
