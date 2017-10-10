#include "observable/class.hpp"
#include "observable/vector.hpp"

#include <string>
#include <vector>

struct element_t 
{
    element_t() = default;
    element_t(std::string p) : s(p) {}
    observable::value<std::string> s;
};

struct oelement_t : observable::class_<
    oelement_t,
    element_t,
    observable::member<element_t,
                       observable::value<std::string>,
                       &element_t::s>
>
{
    using base = observable::class_<
        oelement_t,
        element_t,
        observable::member<element_t,
                           observable::value<std::string>,
                           &element_t::s>
    >;
    using base::base;
};

namespace observable {
    
template<>        
struct is_observable<oelement_t> : std::true_type {};
    
}

using ovector_t = observable::vector<oelement_t>;

int main()
{
    {
        ovector_t ovec;
        ovec.emplace_back(oelement_t{"abc"});
        ovec.push_back(oelement_t{"abc"});
        auto e = oelement_t{"abc"};
        ovec.push_back(std::move(e));
        //TODO
        // ovec.insert(ovec.begin(), {element_t{"abc"}, element_t{"def"}});
        ovec.insert(ovec.begin(), oelement_t{"abc"});
        ovec.emplace(ovec.begin(), "abc");
    }

    {
        ovector_t ovec;
        ovec.push_back(oelement_t{"abc"});
        bool called{false};
        boost::signals2::scoped_connection c =
        ovec.on_value_change
            ([&called](ovector_t::const_iterator it)
             {
                 called = true;
             });
        ovec.front().s = "changed";
        assert(called);
        called = false;
        auto ovec2 = std::move(ovec);
        ovec2.front().s = "changed";
        assert(called);
    }

    {
        ovector_t ovec;
        ovec.emplace_back("abc");
        bool elements_on_change{false};
        bool elements_on_insert{false};
        bool elements_on_value_change{false};
        bool element_on_change{false};
        bool element2_on_change{false};
        bool s_on_change{false};
        bool s2_on_change{false};
        ovec.on_change([&elements_on_change](const ovector_t&)
                       { elements_on_change = true; });
        ovec.on_insert([&elements_on_insert](ovector_t::const_iterator it)
                       {
                           assert(it->s.get() == "ghi");
                           elements_on_insert = true;
                       });
        ovec.push_back(oelement_t{"ghi"});
        assert(elements_on_change);
        assert(elements_on_insert);
    
        ovec.on_value_change([&elements_on_value_change]
                             (ovector_t::const_iterator it)
                             {
                                 assert(it->s.get() == "GHI");
                                 elements_on_value_change = true;
                             });
        auto& oelement = ovec.front();
        oelement.on_change([&element_on_change](const oelement_t&)
                           { element_on_change = true; });
        auto& os = oelement.s;    
        os.on_change([&s_on_change](std::string)
                     { s_on_change = true; });
        {
            auto& oelement = ovec.front();
            oelement.on_change([&element2_on_change](const oelement_t&)
                               { element2_on_change = true; });
            auto& os2 = oelement.s; 
            os2.on_change([&s2_on_change](std::string)
                          { s2_on_change = true; });
        }
        oelement.s = "GHI";
    
        assert(elements_on_value_change);
        assert(element_on_change);
        assert(element2_on_change);
        assert(s_on_change);
        assert(s2_on_change);
    }
}
