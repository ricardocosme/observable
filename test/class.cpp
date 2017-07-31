#include "observable/class.hpp"

#include <iostream>
#include <string>

struct foo_t
{
    double d;
};

struct d{};

using obs_t = observable::class_<
    foo_t,
    observable::value<double, d>
    >;

int main()
{
    //move ctor
    {
        foo_t foo;
        obs_t obs(foo, foo.d);
        auto obs2 = std::move(obs);
        obs2.set<d>(4.5);
    }
    
    //move operator assignment
    {
        foo_t foo;
        obs_t obs(foo, foo.d);
        obs_t obs2(foo, foo.d);
        obs2 = std::move(obs);
        obs2.set<d>(4.5);
    }
}
