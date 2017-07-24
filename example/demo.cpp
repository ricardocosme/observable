#include "observable/from.hpp"
#include "observable/scoped_notifying.hpp"
#include "observable_person.hpp"
#include <iostream>

int main()
{
    person_t model{"maria", 26};    
    auto person = observable::from<observable_person>(model);
    {
        boost::signals2::scoped_connection c1 = person->any_change.connect(
            []{std::cout << "any change" << std::endl;});
        auto sn = observable::make_scoped_notifying(person);
        person->name("MARIA");
        person->age(27);        
    }
    {
        boost::signals2::scoped_connection c1 = person->name_change.connect(
            [](auto new_name)
            {std::cout << "name changed to " << new_name << std::endl;});
        boost::signals2::scoped_connection c2 = person->age_change.connect(
            [](auto new_age)
            {std::cout << "age changed to " << new_age << std::endl;});
        boost::signals2::scoped_connection c3 = person->skills_change.connect(
            []{std::cout << "skills changed" << std::endl;});
        person->name("Maria");
        person->age(28);
        person->skills(
            [](skills_t& skills)
            {
                skills.emplace(8, "woodworking");
                skills.emplace(3, "cooking");
            });
    }
    for(const auto& skill : person->model().skills)
        std::cout << skill.first << " " << skill.second << std::endl;
};
