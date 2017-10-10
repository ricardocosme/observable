#include <observable/value.hpp>

using namespace observable;

struct movable_only_t
{
    movable_only_t(int pvalue) : value(pvalue) {}
    movable_only_t(movable_only_t&&) = default;
    movable_only_t& operator=(const movable_only_t&) = default;
    int value;
};

int main()
{
    {
        value<int> oint;
    }
    
    {
        value<int> oint(10);
        assert(oint == 10);
        assert(10 == oint);
        assert(oint.get() == 10);
        assert(10 == oint.get());
        value<int> oint2(10);
        assert(oint == oint2);
    }

    {
        value<int> oint;
        bool called{false};
        oint.on_change
            ([&called](const int& v)
             {
                 called = true;
                 assert(v == 10);
             });                       
        oint = 10;
        assert(oint == 10);
        assert(called);
    }

    {
        value<int> oint;
        bool called{false};
        oint.on_change
            ([&called](const int& v)
             {
                 called = true;
                 assert(v == 10);
             });                               
        auto oint2 = std::move(oint);
        oint2 = 10;
        assert(oint2 == 10);
        assert(called);
    }

    {
        value<int> oint;
        bool called{false};
        oint.on_change
            ([&called](const int& v)
             {
                 called = true;
                 assert(v == 10);
             });                               
        decltype(oint) oint2;
        oint2 = std::move(oint);
        oint2 = 10;
        assert(oint2 == 10);
        assert(called);
    }

    {
        value<movable_only_t> ovalue(10);
        bool called{false};
        ovalue.on_change
            ([&called](const movable_only_t&)
             {
                 called = true;
             });
        auto extracted = ovalue.extract();
        assert(extracted.value == 10);
        assert(called);
    }
}
