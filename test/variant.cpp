#include "observable/exp/variant.hpp"

#include <array>
#include <iostream>
#include <string>
#include <unordered_set>
#include <boost/mpl/at.hpp>

// struct visitor_t
// {
//     using result_type = void;
    
//     result_type operator()(OInt& o) const
//     { o.assign(10); }
    
//     result_type operator()(OString& o) const
//     { o.assign("hello"); }
// };

// struct visitor_on_change_t
// {
//     using result_type = void;

//     visitor_on_change_t(bool& str_called, bool& num_called)
//         : _str_called(str_called)
//         , _num_called(num_called)
//     {}
    
//     result_type operator()(OInt& o) const
//     {
//         auto& num_called = _num_called;
//         o.on_change([&num_called](int){ num_called = true; });
//     }
//     result_type operator()(OString& o) const
//     {
//         auto& str_called = _str_called;
//         o.on_change([&str_called](const std::string& o){ str_called = true; });
//     }
    
//     bool& _str_called;
//     bool& _num_called;
// };

int main()
{
    observable::exp::variant<int, std::string> variant;
    
    bool variant_on_change{false};
    bool variant_on_change_type{false};
    variant.on_change([&variant_on_change]()
                       { variant_on_change = true;});
    variant.on_change_type([&variant_on_change_type]()
                            { variant_on_change_type = true;});
    
    variant = "hi";
    assert(variant_on_change);
    assert(variant_on_change_type);
    variant_on_change = false;
    variant_on_change_type = false;
    variant = "hello";
    assert(variant_on_change);
    assert(!variant_on_change_type);
    variant_on_change_type = false;
    variant = "hello";
    
    // bool str_on_change{false};
    // bool num_on_change{false};
    // variant.apply_visitor(visitor_on_change_t
    //                        {str_on_change, num_on_change});
    // variant.apply_visitor(visitor_t{});
    // assert(str_on_change);
    
    // variant = 3;    
    // assert(variant_on_change);
    // variant_on_change = false;
    
    // variant.apply_visitor(visitor_on_change_t
    //                        {str_on_change, num_on_change});
    // variant.apply_visitor(visitor_t{});
    // assert(num_on_change);
    // variant_on_change = false;
    
    // variant.match([&variant_on_change](OInt&){variant_on_change = true;},
    //                [](OString&){});
    // assert(variant_on_change);
}

