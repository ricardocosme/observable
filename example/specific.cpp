#include "observable_person.hpp"
#include "person.hpp"
#include <iostream>

int main()
{
    person_t model{"maria", 26};

    observable_person person(model, model.name, model.age, model.skills);
    
    person.on_change<name>(
        [](const std::string& name)
        {std::cout << "name has changed to " << name << std::endl;});

    person.on_change<age>(
        [](const std::size_t& age)
        {std::cout << "age has changed to " << age << std::endl;});
    
    person.on_change<skills>(
        [](const skills_t& skills)
        {std::cout << "skills has changed" << std::endl;});
    
    person.set<name>("bla");
    person.set<age>(45);
    
    person.apply<skills>(
        [](skills_t& skills)
        {
            skills.emplace(8, "woodworking");
        });
};
