#include "observable/variant.hpp"

#include <array>
#include <iostream>
#include <string>
#include <unordered_set>
#include <boost/mpl/at.hpp>

using variant_t = boost::variant<int, std::string>;

using obs_t = observable::variant<variant_t>;

using OInt = observable::observable_of_t<int>;
using OString = observable::observable_of_t<std::string>;

struct visitor_t
{
    using result_type = void;
    
    result_type operator()(OInt& o) const
    { o.assign(10); }
    
    result_type operator()(OString& o) const
    { o.assign("hello"); }
};

struct visitor_on_change_t
{
    using result_type = void;

    visitor_on_change_t(bool& str_called, bool& num_called)
        : _str_called(str_called)
        , _num_called(num_called)
    {}
    
    result_type operator()(OInt& o) const
    {
        auto& num_called = _num_called;
        o.on_change([&num_called](int){ num_called = true; });
    }
    result_type operator()(OString& o) const
    {
        auto& str_called = _str_called;
        o.on_change([&str_called](const std::string& o){ str_called = true; });
    }
    
    bool& _str_called;
    bool& _num_called;
};

int main()
{
    static_assert(observable::is_variant<variant_t>::value, "error");
    variant_t var;
    obs_t ovariant(var);

    bool ovariant_on_change{false};
    bool ovariant_on_change_type{false};
    ovariant.on_change([&ovariant_on_change](const variant_t&)
                       { ovariant_on_change = true;});
    ovariant.on_change_type([&ovariant_on_change_type](const variant_t&)
                            { ovariant_on_change_type = true;});
    
    ovariant = "hi";
    assert(ovariant_on_change);
    assert(ovariant_on_change_type);
    ovariant_on_change = false;
    ovariant_on_change_type = false;
    ovariant = "hello";
    assert(ovariant_on_change);
    assert(!ovariant_on_change_type);
    ovariant_on_change_type = false;
    ovariant = "hello";
    
    bool str_on_change{false};
    bool num_on_change{false};
    ovariant.apply_visitor(visitor_on_change_t
                           {str_on_change, num_on_change});
    ovariant.apply_visitor(visitor_t{});
    assert(str_on_change);
    
    ovariant = 3;    
    assert(ovariant_on_change);
    ovariant_on_change = false;
    
    ovariant.apply_visitor(visitor_on_change_t
                           {str_on_change, num_on_change});
    ovariant.apply_visitor(visitor_t{});
    assert(num_on_change);
    ovariant_on_change = false;
    
    ovariant.match([&ovariant_on_change](OInt&){ovariant_on_change = true;},
                   [](OString&){});
    assert(ovariant_on_change);
}

