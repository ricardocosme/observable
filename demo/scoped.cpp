#include "observable/scoped_on_change.hpp"
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
    
    {
        auto sn = observable::scoped_on_change(person);
        
        person.set<name>("MARIA");
        person.set<age>(27);
    
        person.apply<skills>(
            [](skills_t& skills)
            {
                skills.emplace(8, "woodworking");
            });
    }
};
