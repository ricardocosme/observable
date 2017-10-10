#include <observable/class.hpp>
#include <observable/value.hpp>

using namespace observable;

struct bar_o
{
    value<int> m1;
};

struct bar_t
    : class_<
        bar_t, bar_o,
        member<bar_o, value<int>, &bar_o::m1>
    >
{
};

struct foo_o
{
    value<int> m1;
    value<std::string> m2;
    bar_t m3;
};

struct foo_t
    : class_<
        foo_t, foo_o,
        member<foo_o, value<int>, &foo_o::m1>,
        member<foo_o, value<std::string>, &foo_o::m2>,
        member<foo_o, bar_t, &foo_o::m3>
    >
{    
};

int main()
{
    {
        foo_t foo;
        bool called{false};
        foo.on_change
            ([&called](const foo_t&)
             {
                 called = true;
             });
        foo.m1 = 10;
        assert(called);
        called = false;
        foo.m2 = 10;
        assert(called);
        called = false;
        foo.m3.m1 = 10;
        assert(called);
    }

    {
        foo_t foo;
        bool called{false};
        foo.on_change
            ([&called](const foo_t&)
             {
                 called = true;
             });
        foo.m3.m1 = 10;
        assert(called);
        called = false;
        auto foo2 = std::move(foo);
        foo2.m3.m1 = 5;
        assert(called);
        called = false;
        decltype(foo2) foo3;
        foo3 = std::move(foo2);
        foo3.m3.m1 = 10;
        assert(called);
    }
}
