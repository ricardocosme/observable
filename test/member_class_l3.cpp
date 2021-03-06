#include <observable/class.hpp>
#include <observable/observable_is_class.hpp>
#include <iostream>
#include <type_traits>

struct l3_t
{ double num; };

struct num {};

using ol3_t = observable::class_<
    l3_t,
    std::pair<double, num>
    >;

inline ol3_t observable_factory(l3_t& model)
{
    return ol3_t(model, model.num);
}

OBSERVABLE_IS_CLASS(ol3_t)

struct l2_t
{ l3_t l3; };

struct l3 {};

using ol2_t = observable::class_<
    l2_t,
    std::pair<l3_t, l3>
    >;

inline ol2_t observable_factory(l2_t& model)
{
    return ol2_t(model, model.l3);
}

OBSERVABLE_IS_CLASS(ol2_t)

struct l1_t
{ l2_t l2; };

struct l2 {};

using ol1_t = observable::class_<
    l1_t,
    std::pair<l2_t, l2>
    >;

inline ol1_t observable_factory(l1_t& model)
{
    return ol1_t(model, model.l2);
}

OBSERVABLE_IS_CLASS(ol1_t)

int main()
{
    auto l1 = l1_t{l2_t{{5.6}}};
    auto ol1 = observable_factory(l1);
    auto& ol2 = ol1.get<l2>();
    auto& ol3 = ol2.get<l3>();
    bool l1_on_change_called{false};
    bool l2_on_change_called{false};
    bool l3_on_change_called{false};
    bool l3_num_on_change_called{false};
    
    ol1.on_change([&l1_on_change_called](const l1_t&)
                  { l1_on_change_called = true; });
    
    ol2.on_change([&l2_on_change_called](const l2_t&)
                  { l2_on_change_called = true; });

    ol3.on_change([&l3_on_change_called](const l3_t&)
                  { l3_on_change_called = true; });
    
    ol3.get<num>().on_change([&l3_num_on_change_called](double)
                             { l3_num_on_change_called = true; });    
    ol3.assign<num>(9.5);
    assert(l1_on_change_called);
    assert(l2_on_change_called);
    assert(l3_on_change_called);
    assert(l3_num_on_change_called);
}
