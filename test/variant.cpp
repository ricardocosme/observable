#include "observable/class.hpp"

#include <array>
#include <iostream>
#include <string>
#include <unordered_set>
#include <boost/mpl/at.hpp>

using variant_t = boost::variant<int, std::string>;
struct foo_t
{ variant_t variant; };

struct variant{};
using obs_t = observable::class_<
    foo_t,
    observable::member::variant<variant_t, variant>
    >;

template<typename OVariant>
struct visitor_t
{
    using result_type = void;
    
    result_type operator()
    (observable::member::observable_of_t<OVariant, int>& o) const
    {
        o.set(10);
    }
    result_type operator()
    (observable::member::observable_of_t<OVariant, std::string>& o) const
    {
        o.set("hello");
    }
};

template<typename OVariant>
struct visitor_on_change_t
{
    using result_type = void;
    
    result_type operator()
    (observable::member::observable_of_t<OVariant, int>& o) const
    {
        o.on_change(
            [](const int& o){ std::cout << "has changed to "
                                        << o 
                                        << std::endl; });
    }
    result_type operator()
    (observable::member::observable_of_t<OVariant, std::string>& o) const
    {
        o.on_change(
            [](const std::string& o){ std::cout << "has changed to "
                                                << o
                                                << std::endl; });
    }
};

int main()
{
    foo_t foo;
    obs_t obs(foo, foo.variant);

    auto& ovariant = obs.get<variant>();
    
    using OVariant = std::decay<decltype(ovariant)>::type;
    
    ovariant.set("hi");

    obs.on_change([](const foo_t&){std::cout << "obs has changed" << std::endl;});
    
    ovariant.apply_visitor(visitor_on_change_t<OVariant>{});
    ovariant.apply_visitor(visitor_t<OVariant>{});

    // //apply_visitor
    // {
    //     bool called{false};
    //     auto& omap = obs.get<map>();
    //     omap.emplace(1, "abc");
    //     omap.emplace(2, 100);
    //     auto visitor = visitor_t{called};
    //     omap.at(1).apply_visitor(visitor);
    //     assert(called);
    // }
}
