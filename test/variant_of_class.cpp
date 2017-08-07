#include "observable/class.hpp"

struct foo_t{ int i; };
struct i{};
using ofoo_t = observable::class_<
    foo_t,
    std::pair<int, i>>;

// namespace observable{
ofoo_t factory(foo_t& o)
{
    return ofoo_t(o, o.i);
}
// }
namespace observable{
template<>
struct is_class<foo_t> : std::true_type
{
    using type = ofoo_t;
};
}

struct bar_t{ std::string s; };
struct s{};
using obar_t = observable::class_<
    bar_t,
    std::pair<std::string, s>>;

namespace observable{
template<>
struct is_class<bar_t> : std::true_type
{
    using type = obar_t;
};
}

// namespace observable{
obar_t factory(bar_t& o)
{
    return obar_t(o, o.s);
}
// }
using variant_t = boost::variant<foo_t, bar_t>;
struct variant{};
struct class_variant_t
{ variant_t variant; };
using oclass_variant_t = observable::class_<
    class_variant_t,
    std::pair<variant_t, variant>>;

struct visitor_t
{
    using result_type = void;
    
    result_type operator()(ofoo_t& o) const
    { o.get<i>().set(10); }
    
    result_type operator()(obar_t& o) const
    { o.get<s>().set("hello"); }
};

struct visitor_on_change_t
{
    using result_type = void;

    visitor_on_change_t(bool& str_called, bool& num_called)
        : _str_called(str_called)
        , _num_called(num_called)
    {}
    
    result_type operator()(ofoo_t& o) const
    {
        auto& num_called = _num_called;
        o.on_change([&num_called](const foo_t&){ num_called = true; });
    }
    result_type operator()(obar_t& o) const
    {
        auto& str_called = _str_called;
        o.on_change([&str_called](const bar_t& o){ str_called = true; });
    }
    
    bool& _str_called;
    bool& _num_called;
};

int main()
{
    class_variant_t class_variant;
    oclass_variant_t o(class_variant, class_variant.variant);
    
    auto& ovariant = o.get<variant>();
    
    bool o_on_change{false};
    o.on_change([&o_on_change](const class_variant_t&)
                { o_on_change = true;});
    
    ovariant = bar_t{"hi"};    
    assert(o_on_change);
    
    bool str_on_change{false};
    bool num_on_change{false};
    ovariant.apply_visitor(visitor_on_change_t
                           {str_on_change, num_on_change});
    ovariant.apply_visitor(visitor_t{});
    assert(str_on_change);
    
    // ovariant = 3;    
    // assert(obs_on_change);
    // obs_on_change = false;
    
    // ovariant.apply_visitor(visitor_on_change_t
    //                        {str_on_change, num_on_change});
    // ovariant.apply_visitor(visitor_t{});
    // assert(num_on_change);
}

