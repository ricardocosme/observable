#pragma once

#include <cstddef>
#include <map>
#include <string>
#include <unordered_set>

using skills_t = std::map<
    std::size_t, /*skill level*/
    std::string  /*skill name*/
>;

using kids_t = std::unordered_set<std::string>;

struct person_t
{
    std::string name;
    std::size_t age;
    skills_t skills;
    kids_t kids;
};
