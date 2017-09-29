#include <observable/exp/class.hpp>
#include <observable/exp/set_get.hpp>
#include <iostream>
using namespace observable::exp;

class foo_t : public observable::exp::class_
{
    std::string _s;
public:    
    foo_t()
    {
        s_signal.connect([this](std::string){on_change()();});
    }

    void set(std::string s)
    {
        _s = std::move(s);
        s_signal(_s);
    }
    
    std::string get() const noexcept
    { return _s; }

    boost::signals2::signal<void(std::string)> s_signal;
};

int main()
{
    foo_t foo;
    bool value_changed{false};
    foo.on_change
        ([&value_changed]()
         { value_changed = true; });
    foo.set("something");
    assert(value_changed);
    assert(foo.get() == "something");
}
