#include "observable_person.hpp"
#include "person.hpp"
#include <iostream>

int main()
{
    person_t model{"maria", 26};

    observable_person person(model, model.name, model.age, model.skills);
    
    person.on_change(
        []{std::cout << "any change" << std::endl;});
    
    person.set<name>("bla");
    person.set<age>(45);
    
    person.apply<skills>(
        [](skills_t& skills)
        {
            skills.emplace(8, "woodworking");
        });
};
