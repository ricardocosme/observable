
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

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

/// This class models a person without any support to a notification
/// system like signal/slot.

struct person_t
{
    std::string name;
    std::size_t age;
    skills_t skills;
    kids_t kids;
};
