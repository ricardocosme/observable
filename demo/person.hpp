#pragma once

#include <cstddef>
#include <map>
#include <string>

using skills_t = std::map<
    std::size_t, /*skill level*/
    std::string  /*skill name*/
>;

struct person_t
{
    std::string name;
    std::size_t age;
    skills_t skills;
};
