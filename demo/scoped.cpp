#include "observable/scoped_on_change.hpp"
#include "observable_person.hpp"
#include "person.hpp"
#include <iostream>

int main()
{
    person_t model{"maria", 26};

    auto person = observable::factory(model);
    
    person.on_change([]{std::cout << "person has changed" << std::endl;});
    
    {
        auto sn = observable::scoped_on_change(person);
        
        person.set<name>("bla");
        person.set<age>(45);
    
        person.apply<skills>(
            [](skills_t& skills)
            {
                skills.emplace(8, "woodworking");
            });
    }
};
