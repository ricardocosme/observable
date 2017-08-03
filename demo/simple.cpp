
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
    auto operson = observable::factory(model);

    /// Register a closure to react to any change of person
    operson.on_change(
        [](const person_t& p){std::cout << "person " << p.name
                                        << " has changed" << std::endl;});

    /// Change the person's name. This action may emit the following
    /// signals: operson.on_change and operson.name.on_change
    operson.set<name>("MARIA");
    
    /// Change the person's age. This action may emit the following
    /// signals: operson.on_change and operson.age.on_change
    operson.set<age>(27);

    /// Get the observable of operson.skills, which is an
    /// observable::map, and insert a new element to the
    /// container. This action may emit the following signals:
    /// operson.on_change, operson.skills.on_change and
    /// operson.skills.on_insert.
    operson.get<skills>().emplace(8, "woodworking");
    operson.get<skills>().emplace(7, "cooking");
    
    /// Get the observable of operson.kids, which is an
    /// observable::unordered_set, and insert a new element to the
    /// container. This action may emit the following signals:
    /// operson.on_change, operson.kids.on_change and
    /// operson.kids.on_insert.
    operson.get<kids>().emplace("josefina");

    /// Get the observable of an specific container's element and
    /// change the value. This action may emit the following signals:
    /// operson.on_change, operson.skills.on_change, 
    /// operson.skills.on_value_change and the element's on_change.
    auto o = operson.get<skills>().at(8);
    o->set("Woodworking");

    /// A const lvalue reference to the model can be obtained through
    /// the method model()
    for(auto p : operson.get<skills>().model())
        std::cout << p.second << std::endl;    
}
