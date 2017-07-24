#include "observable_person.hpp"
#include "person.hpp"
#include <iostream>

int main()
{
    person_t model{"maria", 26};

    auto person = observable::factory(model);
    
    person.on_change(
        []{std::cout << "any change" << std::endl;});
    
    person.set<name>("MARIA");
    person.set<age>(27);
    
    person.apply<skills>(
        [](skills_t& skills)
        {
            skills.emplace(8, "woodworking");
            skills.emplace(7, "cooking");
        });
};
