#include "observable_person.hpp"
#include "person.hpp"
#include <iostream>

int main()
{
    person_t model{"maria", 26};

    auto person = observable::factory(model);
    
    person.on_change(
        [](const person_t& p){std::cout << "person " << p.name
                                        << " has changed" << std::endl;});
    
    person.set<name>("MARIA");
    person.set<age>(27);
    
    person.apply<skills>(
        [](skills_t& skills)
        {
            skills.emplace(8, "woodworking");
            skills.emplace(7, "cooking");
        });
    
    std::cout << model.name << std::endl
              << model.age << std::endl;
};
