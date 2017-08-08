#include "observable/class.hpp"

#include <iostream>
#include <string>
#include <vector>

using vector_t = std::vector<std::vector<std::string>>;
struct foo_t { vector_t elements; };

struct elements{};
using ofoo_t = observable::class_<
    foo_t,
    std::pair<vector_t, elements>
    >;

ofoo_t observable_factory(foo_t& model)
{ return ofoo_t(model, model.elements); }

int main()
{
    std::vector<std::string> e0{"abc"};
    std::vector<std::string> e1{"def"};
    foo_t foo{{e0, e1}};
    auto ofoo = observable_factory(foo);
    auto& oelements = ofoo.get<elements>();
    bool foo_on_change{false};
    bool elements_on_change{false};
    bool elements_on_insert{false};
    // bool elements_on_value_change{false};
    // bool element_on_change{false};
    // bool element2_on_change{false};
    // bool s_on_change{false};
    // bool s2_on_change{false};
    ofoo.on_change([&foo_on_change](const foo_t&)
                   { foo_on_change = true; });
    oelements.on_change([&elements_on_change](const vector_t&)
                        { elements_on_change = true; });
    oelements.on_insert([&elements_on_insert](const vector_t&,
                                              vector_t::const_iterator it)
                        {
                            assert(it->front() == "ghi");
                            elements_on_insert = true;
                        });
    oelements.push_back(std::vector<std::string>{"ghi"});
    assert(foo_on_change);
    assert(elements_on_change);
    assert(elements_on_insert);
    
    // oelements.on_value_change([&elements_on_value_change]
    //                           (const vector_t&,
    //                            vector_t::const_iterator it)
    //                           {
    //                               assert(it->s == "GHI");
    //                               elements_on_value_change = true;
    //                           });
    // auto oelement = oelements.front();
    // oelement->on_change([&element_on_change](const element_t&)
    //                    { element_on_change = true; });
    // auto& os = oelement->get<s>();    
    // os.on_change([&s_on_change](std::string)
    //              { s_on_change = true; });
    // {
    //     auto oelement = oelements.front();
    //     oelement->on_change([&element2_on_change](const element_t&)
    //                         { element2_on_change = true; });
    //     auto& os2 = oelement->get<s>();    
    //     os2.on_change([&s2_on_change](std::string)
    //                   { s2_on_change = true; });
    // }
    // oelement->get<s>().assign("GHI");
    // os.assign("GHI");
    
    // assert(elements_on_value_change);
    // assert(element_on_change);
    // assert(element2_on_change);
    // assert(s_on_change);
    // assert(s2_on_change);
}
