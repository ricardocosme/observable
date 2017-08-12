#include <observable/class.hpp>
#include <observable/observable_class_gen.hpp>
#include <observable/setter_value.hpp>
#include <iostream>

using namespace observable;

struct foo_t {
    void val(float v)
    { _v = v;
        std::cout << &_v << std::endl;}
    
    const float& val() const noexcept
    { return _v; }
private:
    float _v;
};

struct val{};
using ofoo_t = class_<
    foo_t,
    std::pair<set_get<float>, val>>;        

ofoo_t observable_factory(foo_t& o)
{
    return ofoo_t(o,
                  set_get<float>{
                      [&o](float v){o.val(v);},
                      [&o]() -> const float& {return o.val();}
                  });
}

int main()
{
    foo_t foo;
    auto ofoo = observable_factory(foo);
    bool value_changed{false};
    ofoo.on_change<val>(
        [&value_changed](float v)
        { value_changed = true; });
    ofoo.get<val>() = 7.6;
    assert(value_changed);
    assert((ofoo.get<val>().get() == 7.6) < 0.01);
}
