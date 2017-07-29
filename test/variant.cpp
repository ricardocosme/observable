#include "observable/class.hpp"
#include "observable/map.hpp"

#include <array>
#include <iostream>
#include <string>
#include <unordered_set>

using variant_t = boost::variant<int, std::string>;
using map_t = std::map<std::size_t, variant_t>;

struct foo_t
{
    map_t map;
};

struct map{};

using obs_t = observable::class_<
    foo_t,
    observable::map<map_t, observable::variant, map>
    >;

struct visitor_t
{
    using result_type = void;
    
    visitor_t(bool& called) : _called(called) {}
    
    template<typename T>
    result_type operator()(T&&)
    { _called = true; }
    
    bool& _called;
};

int main()
{
    foo_t foo;
    obs_t obs(foo, foo.map);

    //apply_visitor
    {
        bool called{false};
        auto& omap = obs.get<map>();
        omap.emplace(1, "abc");
        omap.emplace(2, 100);
        auto visitor = visitor_t{called};
        omap.at(1).apply_visitor(visitor);
        assert(called);
    }
}
