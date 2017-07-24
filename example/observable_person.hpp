#pragma once

#include "observable/from.hpp"
#include "observable/scoped_notifying.hpp"
#include "person.hpp"
#include <boost/signals2.hpp>

class observable_person
{
private:
    person_t& _person;
    bool _under_transaction{false};
    template <typename> friend class observable::scoped_notifying;
public:    
    using Model = person_t;    
    
    observable_person(person_t& person)
        : _person(person) {}
    
    void name(std::string s)
    {
        _person.name = std::move(s);
        if (!_under_transaction)
        {
            name_change(_person.name);
            any_change();
        }
    }

    std::string name() const noexcept
    { return _person.name; }
    
    void age(std::size_t val)
    {
        _person.age = val;
        if (!_under_transaction)
        {
            age_change(_person.age);
            any_change();
        }
    }

    std::size_t age() const noexcept
    { return _person.age; }

    template<typename F>
    void skills(F&& f)
    {
        f(_person.skills);
        if (!_under_transaction)
        {
            skills_change();
            any_change();
        }        
    }

    const person_t& model() const noexcept
    { return _person; }

    boost::signals2::signal<void(std::string)> name_change;
    boost::signals2::signal<void(std::size_t)> age_change;
    boost::signals2::signal<void()> skills_change;
    boost::signals2::signal<void()> any_change;    
};
