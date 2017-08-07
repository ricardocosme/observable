
// Copyright Ricardo Calheiros de Miranda Cosme 2017.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "observable_person.hpp"
#include "person.hpp"
#include <iostream>
#include <boost/fusion/include/at_key.hpp>
#include <boost/fusion/include/make_map.hpp>

int main()
{
    /// Create a model of person
    person_t model{"maria", 26};

    /// Get an observable of person
    auto operson = observable_factory(model);

    /// Register a closure to react to any change of person
    operson.on_change(
        [](const person_t& p)
        {std::cout << "person " << p.name
                   << " has changed" << std::endl;});

    /// Register a closure to react to change of person's name
    operson.on_change<name>(
        [](const std::string& name)
        {std::cout << "person's name has changed to "
                   << name << std::endl;});
    
    /// Change the person's name. This action emits the following signals:
    /// operson.on_change and operson.name.on_change
    operson.set<name>("MARIA");

    /// Get the observable of person.skills, which is an
    /// observable::map
    auto& oskills = operson.get<skills>();
    
    /// Register a closure to react to new skills
    oskills.on_insert(
        [](const skills_t&, skills_t::const_iterator it)
        {std::cout << "new skill was added: "
                   << it->second << " with level "
                   << it->first << std::endl;});
    
    /// Add a new skill. This action emits the following signals:
    /// operson.skills.on_insert and operson.on_change
    oskills.emplace(8, "woodworking");
    
    /// Register a closure to react to changes of a skill
    oskills.on_value_change(
        [](const skills_t&, skills_t::const_iterator it)
        {std::cout << "skill has changed to "
                   << it->second << " with level "
                   << it->first << std::endl;});
    
    /// Get the observable of an specific container's element and
    /// change the value. This action emits the following signals:
    /// operson.skills.on_value_change and operson.on_change
    auto o = oskills.at(8);
    o->set("Woodworking");
}
