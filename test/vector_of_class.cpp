#include "observable/class.hpp"
#include "observable/observable_is_class.hpp"

#include <iostream>
#include <string>
#include <vector>

struct element_t{ std::string s; };
using vector_t = std::vector<element_t>;
struct foo_t
{ vector_t elements; };

struct s{};
using oelement_t = observable::class_<
    element_t,
    std::pair<std::string, s>
    >;

oelement_t observable_factory(element_t& model)
{ return oelement_t(model, model.s); }

OBSERVABLE_IS_CLASS(oelement_t)
    
struct elements{};
using ofoo_t = observable::class_<
    foo_t,
    std::pair<vector_t, elements>
    >;

ofoo_t observable_factory(foo_t& model)
{ return ofoo_t(model, model.elements); }

int main()
{
    element_t e0{"abc"};
    element_t e1{"def"};
    foo_t foo{{e0, e1}};
    auto ofoo = observable_factory(foo);
    auto& oelements = ofoo.get<elements>();
    bool foo_on_change{false};
    bool elements_on_change{false};
    bool elements_on_insert{false};
    bool elements_on_value_change{false};
    bool element_on_change{false};
    bool element2_on_change{false};
    bool s_on_change{false};
    bool s2_on_change{false};
    ofoo.on_change([&foo_on_change](const foo_t&)
                   { foo_on_change = true; });
    oelements.on_change([&elements_on_change](const vector_t&)
                        { elements_on_change = true; });
    oelements.on_insert([&elements_on_insert](const vector_t&,
                                              vector_t::const_iterator it)
                        {
                            assert(it->s == "ghi");
                            elements_on_insert = true;
                        });
    oelements.push_back(element_t{"ghi"});
    assert(foo_on_change);
    assert(elements_on_change);
    assert(elements_on_insert);
    
    oelements.on_value_change([&elements_on_value_change]
                              (const vector_t&,
                               vector_t::const_iterator it)
                              {
                                  assert(it->s == "GHI");
                                  elements_on_value_change = true;
                              });
    auto oelement = oelements.front();
    oelement->on_change([&element_on_change](const element_t&)
                       { element_on_change = true; });
    auto& os = oelement->get<s>();    
    os.on_change([&s_on_change](std::string)
                 { s_on_change = true; });
    {
        auto oelement = oelements.front();
        oelement->on_change([&element2_on_change](const element_t&)
                            { element2_on_change = true; });
        auto& os2 = oelement->get<s>();    
        os2.on_change([&s2_on_change](std::string)
                      { s2_on_change = true; });
    }
    oelement->get<s>().set("GHI");
    os.set("GHI");
    
    assert(elements_on_value_change);
    assert(element_on_change);
    assert(element2_on_change);
    assert(s_on_change);
    assert(s2_on_change);
}
