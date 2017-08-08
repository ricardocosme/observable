#include "observable_person.hpp"
#include "person.hpp"
#include <iostream>

struct operson_t : observable_person
{
    using base = observable_person;
    
    operson_t(person_t model)
        : base(observable_factory(_model))
        , _model(std::move(model))
    {}
private:
    person_t _model;
};

int main()
{
    operson_t person{person_t{"maria", 26}};

    person.on_change(
        [](const person_t& name)
        {std::cout << "person has changed" << std::endl;});
    
    person.get<kids>().on_insert(
        [](const kids_t& skills)
        {std::cout << "kids has inserted" << std::endl;});
    
    person.assign<name>("MARIA");
    person.assign<age>(27);

    auto& okids = person.get<kids>();
    okids.emplace("josefina");
    okids.emplace("lucas");

    const auto& mkids = okids.get();
    for(const auto& kid : mkids)
        std::cout << "kid: " << kid << std::endl;
}
