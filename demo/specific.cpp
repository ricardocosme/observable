#include "observable_person.hpp"
#include "person.hpp"
#include <iostream>

int main()
{
    person_t model{"maria", 26};

    auto person = observable_factory(model);
    
    person.on_change<name>(
        [](const std::string& name)
        {std::cout << "name has changed to " << name << std::endl;});

    person.on_change<age>(
        [](const std::size_t& age)
        {std::cout << "age has changed to " << age << std::endl;});
    
    person.on_change<skills>(
        [](const skills_t& skills)
        {std::cout << "skills has changed" << std::endl;});
    
    person.get<kids>().on_insert(
        [](const kids_t& skills)
        {std::cout << "kids has inserted" << std::endl;});
    
    person.set<name>("MARIA");
    person.set<age>(27);
    
    person.get<skills>().emplace(8, "woodworking");    
    person.get<kids>().emplace("josefina");
}
