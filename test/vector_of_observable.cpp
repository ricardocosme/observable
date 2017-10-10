#include <observable/value.hpp>
#include <observable/vector.hpp>

#include <string>
#include <vector>
#include <iostream>
using namespace observable;

using ovec_t = vector<value<std::string>>;

template<typename ocontainer, typename value_type>
void check_equal(ocontainer& c, std::initializer_list<value_type> il)
{
    auto it = il.begin();
    assert(c.size() == il.size());
    for(auto& e : c)
        assert(e.get() == *it++);
}

int main()
{
    {
        ovec_t vec;        
    }

    //vector(size_type)
    {
        ovec_t vec(5);
        assert(vec.size() == 5);
        for(auto& e : vec)
        {
            bool on_value_change_called{false};
            boost::signals2::scoped_connection c1 = 
            vec.on_value_change
                ([&on_value_change_called](const ovec_t&,
                                           const ovec_t::value_type& e)
                 {
                     assert(e == std::string{"changed"});
                     on_value_change_called = true;
                 });
            bool on_change_called{false};
            boost::signals2::scoped_connection c2 = 
            vec.on_change
                ([&on_change_called](const ovec_t&)
                 {
                     on_change_called = true;
                 });                                
            assert(e == std::string{});
            e = std::string{"changed"};
            assert(on_value_change_called);
            assert(on_change_called);
        }
        
        auto vec2 = std::move(vec);
        for(auto& e : vec2)
        {
            bool on_value_change_called{false};
            boost::signals2::scoped_connection c1 = 
            vec2.on_value_change
                ([&on_value_change_called](const ovec_t&,
                                           const ovec_t::value_type& e)
                 {
                     assert(e == std::string{"another_change"});
                     on_value_change_called = true;
                 });
            bool on_change_called{false};
            boost::signals2::scoped_connection c2 = 
            vec2.on_change
                ([&on_change_called](const ovec_t&)
                 {
                     on_change_called = true;
                 });                                
            assert(e == std::string{"changed"});
            e = std::string{"another_change"};
            assert(on_value_change_called);
            assert(on_change_called);
        }

        decltype(vec2) vec3;
        vec3 = std::move(vec2);
        
        for(auto& e : vec3)
        {
            bool on_value_change_called{false};
            boost::signals2::scoped_connection c1 = 
            vec3.on_value_change
                ([&on_value_change_called](const ovec_t&,
                                           const ovec_t::value_type& e)
                 {
                     assert(e == std::string{"yet_another_change"});
                     on_value_change_called = true;
                 });
            bool on_change_called{false};
            boost::signals2::scoped_connection c2 = 
            vec3.on_change
                ([&on_change_called](const ovec_t&)
                 {
                     on_change_called = true;
                 });                                
            assert(e == std::string{"another_change"});
            e = std::string{"yet_another_change"};
            assert(on_value_change_called);
            assert(on_change_called);
        }
    }
    
    //vector(first, last)
    {
        std::vector<std::string> src({"abc", "def"});
        ovec_t vec(src.begin(), src.end());
        assert(vec.size() == 2);
        check_equal(vec, {"abc", "def"});
        for(auto& e : vec)
        {
            bool on_value_change_called{false};
            boost::signals2::scoped_connection c1 = 
            vec.on_value_change
                ([&on_value_change_called](const ovec_t&,
                                           const ovec_t::value_type& e)
                 {
                     assert(e == std::string{"changed"});
                     on_value_change_called = true;
                 });
            bool on_change_called{false};
            boost::signals2::scoped_connection c2 = 
            vec.on_change
                ([&on_change_called](const ovec_t&)
                 {
                     on_change_called = true;
                 });                                
            e = std::string{"changed"};
            assert(on_value_change_called);
            assert(on_change_called);
        }
        
        auto vec2 = std::move(vec);
        for(auto& e : vec2)
        {
            bool on_value_change_called{false};
            boost::signals2::scoped_connection c1 = 
            vec2.on_value_change
                ([&on_value_change_called](const ovec_t&,
                                           const ovec_t::value_type& e)
                 {
                     assert(e == std::string{"another_change"});
                     on_value_change_called = true;
                 });
            bool on_change_called{false};
            boost::signals2::scoped_connection c2 = 
            vec2.on_change
                ([&on_change_called](const ovec_t&)
                 {
                     on_change_called = true;
                 });                                
            e = std::string{"another_change"};
            assert(on_value_change_called);
            assert(on_change_called);
        }

        decltype(vec2) vec3;
        vec3 = std::move(vec2);
        
        for(auto& e : vec3)
        {
            bool on_value_change_called{false};
            boost::signals2::scoped_connection c1 = 
            vec3.on_value_change
                ([&on_value_change_called](const ovec_t&,
                                           const ovec_t::value_type& e)
                 {
                     assert(e == std::string{"yet_another_change"});
                     on_value_change_called = true;
                 });
            bool on_change_called{false};
            boost::signals2::scoped_connection c2 = 
            vec3.on_change
                ([&on_change_called](const ovec_t&)
                 {
                     on_change_called = true;
                 });                                
            e = std::string{"yet_another_change"};
            assert(on_value_change_called);
            assert(on_change_called);
        }
    }
        
    //begin()
    {
        ovec_t vec;
        vec.emplace_back("abc");
        assert(*vec.begin() == std::string{"abc"});
    }

    //cbegin()
    {
        ovec_t vec;
        vec.emplace_back("abc");
        assert(*vec.cbegin() == std::string{"abc"});
    }
    
    //rbegin()
    {
        ovec_t vec;
        vec.emplace_back("abc");
        vec.emplace_back("def");
        assert(*vec.rbegin() == std::string{"def"});
    }
    
    //crbegin()
    {
        ovec_t vec;
        vec.emplace_back("abc");
        vec.emplace_back("def");
        assert(*vec.crbegin() == std::string{"def"});
    }
    
    //end()
    {
        ovec_t vec;
        vec.emplace_back("abc");
        vec.emplace_back("def");
        assert(std::distance(vec.begin(), vec.end()) == 2);
    }

    //cend()
    {
        ovec_t vec;
        vec.emplace_back("abc");
        vec.emplace_back("def");
        assert(std::distance(vec.cbegin(), vec.cend()) == 2);
    }    

    //empty()
    {
        ovec_t vec;
        assert(vec.empty());
        vec.emplace_back("something");
        assert(!vec.empty());        
    }
    
    //size()
    {
        ovec_t vec;
        vec.emplace_back("abc");
        vec.emplace_back("def");
        assert(vec.size() == 2);
    }
    
    //max_size()
    {
        ovec_t vec;
        vec.emplace_back("abc");
        vec.emplace_back("def");
        assert(vec.max_size() >= 2);
    }
    
    //reserve()
    {
        ovec_t vec;
        vec.reserve(100);
    }
    
    //capacity()
    {
        ovec_t vec;
        vec.emplace_back("abc");
        assert(vec.capacity() >= 1);
    }

    //shrink_to_fit()
    {
        ovec_t vec;
        vec.emplace_back("abc");
        vec.shrink_to_fit();
    }
    
    //at fail
    {
        ovec_t vec;
        bool ok{false};
        try
        {
            vec.at(0);
        }
        catch(const std::out_of_range&)
        { ok = true; }
        assert(ok);
    }

    //at success
    {
        bool ok{true};
        ovec_t vec;
        vec.emplace(vec.begin(), "abc");
        try
        {
            assert(vec.at(0) == std::string{"abc"});
            bool on_value_change_called{false};
            bool on_change_called{false};
            vec.on_value_change
                ([&on_value_change_called](const ovec_t&,
                                           const ovec_t::value_type& e)
                 {
                     assert(e == std::string{"change"});
                     on_value_change_called = true;
                 });
            vec.on_change
                ([&on_change_called](const ovec_t&)
                 {
                     on_change_called = true;
                 });
            vec.front() = "change";
            assert(on_value_change_called);
            assert(on_change_called);
        }
        catch(const std::out_of_range&)
        { ok = false; }
        assert(ok);
    }
    
    //at const fail
    {
        ovec_t vec;
        bool ok{false};
        try
        {
            const auto& cvalue = vec;
            cvalue.at(0);
        }
        catch(const std::out_of_range&)
        { ok = true; }
        assert(ok);
    }

    //at const success
    {
        bool ok{true};
        ovec_t vec;
        vec.emplace(vec.begin(), "abc");
        try
        {
            const auto& cvec = vec;
            auto& cvalue = cvec.at(0);
            assert(cvalue == std::string{"abc"});
        }
        catch(const std::out_of_range&)
        { ok = false; }
        assert(ok);            
    }

    //operator[] 
    {
        ovec_t vec;
        vec.emplace(vec.begin(), "abc");
        assert(vec[0] == std::string{"abc"});
        bool on_value_change_called{false};
        bool on_change_called{false};
        vec.on_value_change
            ([&on_value_change_called](const ovec_t&,
                                       const ovec_t::value_type& e)
             {
                 assert(e == std::string{"change"});
                 on_value_change_called = true;
             });
        vec.on_change
            ([&on_change_called](const ovec_t&)
             {
                 on_change_called = true;
             });
        vec[0] = "change";
        assert(on_value_change_called);
        assert(on_change_called);
    }

    //operator[] const
    {
        ovec_t vec;
        vec.emplace(vec.begin(), "abc");
        const auto& cvec = vec;
        assert(cvec[0] == std::string{"abc"});
    }
    
    //front()
    {
        ovec_t vec;
        vec.emplace(vec.begin(), "abc");
        assert(vec.front() == std::string{"abc"});
        bool on_value_change_called{false};
        bool on_change_called{false};
        vec.on_value_change
            ([&on_value_change_called](const ovec_t&,
                                       const ovec_t::value_type& e)
             {
                 assert(e == std::string{"change"});
                 on_value_change_called = true;
             });
        vec.on_change
            ([&on_change_called](const ovec_t&)
             {
                 on_change_called = true;
             });
        vec.front() = "change";
        assert(on_value_change_called);
        assert(on_change_called);
    }
    
    //front() const
    {
        ovec_t vec;
        vec.emplace(vec.begin(), "abc");
        auto& cvec = vec;
        assert(cvec.front() == std::string{"abc"});
    }
    
    //back()
    {
        ovec_t vec;
        vec.emplace_back("abc");
        vec.emplace_back("def");
        assert(vec.back() == std::string{"def"});
        bool on_value_change_called{false};
        bool on_change_called{false};
        vec.on_value_change
            ([&on_value_change_called](const ovec_t&,
                                       const ovec_t::value_type& e)
             {
                 assert(e == std::string{"change"});
                 on_value_change_called = true;
             });
        vec.on_change
            ([&on_change_called](const ovec_t&)
             {
                 on_change_called = true;
             });
        vec.back() = "change";
        assert(on_value_change_called);
        assert(on_change_called);
    }
    
    //back() const
    {
        ovec_t vec;
        vec.emplace_back("abc");
        vec.emplace_back("def");
        const auto& cvec = vec;
        assert(cvec.back() == std::string{"def"});
    }
        
    //clear() before_erase returns true
    {
        ovec_t vec;
        vec.emplace_back("abc");
        vec.emplace_back("def");
        bool after_called{false};
        bool before_called{false};
        bool on_change_called{false};
        vec.before_erase([&before_called]
                         (const ovec_t& vec,
                          ovec_t::const_iterator)
                         {
                             before_called = true;
                             return true;
                         });
        vec.after_erase([&after_called]
                        (const ovec_t&,
                         ovec_t::const_iterator)
                        { after_called = true; });
        vec.on_change([&on_change_called]
                      (const ovec_t&)
                      { on_change_called = true; });
        assert(vec.clear());
        assert(before_called);
        assert(after_called);
        assert(on_change_called);
    }
            
    //clear() before_erase returns false
    {
        ovec_t vec;
        vec.emplace_back("abc");
        vec.emplace_back("def");
        bool after_called{false};
        bool before_called{false};
        bool on_change_called{false};
        vec.before_erase([&before_called]
                         (const ovec_t& vec,
                          ovec_t::const_iterator it)
                         {
                             before_called = true;
                             assert(vec.cend() == it);
                             return false;
                         });
        vec.after_erase([&after_called]
                        (const ovec_t&,
                         const ovec_t::const_iterator)
                        { after_called = true; });
        vec.on_change([&on_change_called]
                      (const ovec_t&)
                      { on_change_called = true; });
        assert(!vec.clear());
        assert(before_called);
        assert(!after_called);
        assert(!on_change_called);
    }
    
    //erase() before_erase returns true
    {
        ovec_t vec;
        vec.emplace_back("abc");
        vec.emplace_back("def");
        bool after_called{false};
        bool before_called{false};
        bool on_change_called{false};
        vec.before_erase([&before_called]
                         (const ovec_t& vec,
                          ovec_t::const_iterator it)
                         {
                             before_called = true;
                             assert(*it == std::string{"abc"});
                             return true;
                         });
        vec.after_erase([&after_called]
                        (const ovec_t&,
                         ovec_t::const_iterator)
                        { after_called = true; });
        vec.on_change([&on_change_called]
                      (const ovec_t&)
                      { on_change_called = true; });
        auto ret = vec.erase(vec.begin());
        assert(ret.second);
        assert(*ret.first == std::string{"def"});
        assert(vec.size() == 1);
        assert(before_called);
        assert(after_called);
        assert(on_change_called);
    }
    
    //erase() before_erase returns false
    {
        ovec_t vec;
        vec.emplace_back("abc");
        vec.emplace_back("def");
        bool after_called{false};
        bool before_called{false};
        bool on_change_called{false};
        vec.before_erase([&before_called]
                         (const ovec_t& vec,
                          ovec_t::const_iterator it)
                         {
                             before_called = true;
                             assert(*it == std::string{"abc"});
                             return false;
                         });
        vec.after_erase([&after_called]
                        (const ovec_t&,
                         ovec_t::const_iterator)
                        { after_called = true; });
        vec.on_change([&on_change_called]
                      (const ovec_t&)
                      { on_change_called = true; });
        auto ret = vec.erase(vec.begin());
        assert(ret.second == false);
        assert(ret.first == vec.end());
        assert(vec.size() == 2);
        assert(before_called);
        assert(!after_called);
        assert(!on_change_called);
    }
    
    //erase(first, last) before_erase returns true
    {
        ovec_t vec;
        vec.emplace_back("abc");
        vec.emplace_back("def");
        bool called{false};
        bool before_called{false};
        bool on_change_called{false};
        vec.before_erase([&before_called]
                          (const ovec_t& vec,
                           ovec_t::const_iterator it)
                         {
                             before_called = true;
                             assert(vec.cbegin() == it);
                             return true;
                         });
        vec.after_erase([&called](const ovec_t&,
                                  ovec_t::const_iterator)
                        { called = true; });
        vec.on_change([&on_change_called]
                      (const ovec_t&)
                      { on_change_called = true; });
        auto ret = vec.erase(vec.begin(), vec.end());
        assert(ret.second);
        assert(ret.first == vec.end());
        assert(before_called);
        assert(called);
        assert(on_change_called);
        assert(vec.empty());
    }
    
    //erase(first, last) before_erase returns false
    {
        ovec_t vec;
        vec.emplace_back("abc");
        vec.emplace_back("def");
        bool called{false};
        bool before_called{false};
        bool on_change_called{false};
        vec.before_erase([&before_called]
                          (const ovec_t& vec,
                           ovec_t::const_iterator it)
                         {
                             before_called = true;
                             assert(vec.cbegin() == it);
                             return false;
                         });
        vec.after_erase([&called](const ovec_t&,
                                  ovec_t::const_iterator)
                        { called = true; });
        vec.on_change([&on_change_called]
                      (const ovec_t&)
                      { on_change_called = true; });
        auto ret = vec.erase(vec.begin(), vec.end());
        assert(!ret.second);
        assert(ret.first == vec.end());
        assert(before_called);
        assert(!called);
        assert(!on_change_called);
        assert(!vec.empty());
    }
    
    //emplace()
    {
        ovec_t vec;
        bool called{false};
        bool on_change_called{false};
        vec.emplace_back("abc");
        vec.after_insert([&called](const ovec_t&,
                                   ovec_t::const_iterator it)
                       {
                           assert(*it == std::string{"def"});
                           called = true;
                       });
        vec.on_change([&on_change_called]
                      (const ovec_t&)
                      { on_change_called = true; });
        vec.emplace(vec.begin(), "def");
        assert(called);
        assert(on_change_called);
        assert(vec.front() == std::string{"def"});
        assert(vec.size() == 2);
    }
    
    //insert(pos, value_type&&)
    {
        ovec_t vec;
        bool called{false};
        bool on_change_called{false};
        std::string v = "abc";
        vec.insert(vec.begin(), v);
        vec.after_insert([&called](const ovec_t&,
                                   ovec_t::const_iterator it)
                         {
                             assert(*it == std::string{"def"});
                             called = true;
                         });
        vec.on_change([&on_change_called]
                      (const ovec_t&)
                      { on_change_called = true; });
        std::string v2 = "def";
        vec.insert(std::next(vec.begin()), v2);
        assert(called);
        assert(on_change_called);
        assert(vec.front() == std::string{"abc"});
        assert(*std::next(vec.cbegin()) == std::string{"def"});
        assert(vec.size() == 2);
    }
    
    //insert(pos, first, last)
    {
        ovec_t vec;
        bool called{false};
        bool on_change_called{false};
        vec.after_insert([&called](const ovec_t&,
                                   ovec_t::const_iterator it)
                         {
                             assert(*it == std::string{"abc"});
                             called = true;
                         });
        vec.on_change([&on_change_called]
                      (const ovec_t&)
                      { on_change_called = true; });
        auto it = vec.begin();
        std::vector<std::string> src{"abc", "def"};
        vec.insert(it, src.begin(), src.end());
        assert(called);
        assert(on_change_called);
        assert(*std::next(vec.cbegin()) == std::string{"def"});
        assert(vec.size() == 2);
    }

    //push_back()
    {
        ovec_t vec;
        bool called{false};
        bool on_change_called{false};
        vec.emplace_back("abc");
        vec.after_insert([&called]
                         (const ovec_t&,
                          ovec_t::const_iterator it)
                         {
                             assert(*it == std::string{"def"});
                             called = true;
                         });
        vec.on_change([&on_change_called]
                      (const ovec_t&)
                      { on_change_called = true; });
        vec.push_back(std::string{"def"});
        assert(called);
        assert(on_change_called);
        assert(vec.front() == std::string{"abc"});
        assert(vec.back() == std::string{"def"});
    }
    
    //emplace_back()
    {
        ovec_t vec;
        bool called{false};
        bool on_change_called{false};
        vec.emplace_back("abc");
        vec.after_insert([&called]
                         (const ovec_t&,
                          ovec_t::const_iterator it)
                         {
                             assert(*it == std::string{"def"});
                             called = true;
                         });
        vec.on_change([&on_change_called]
                      (const ovec_t&)
                      { on_change_called = true; });
        vec.emplace_back("def");
        assert(called);
        assert(on_change_called);
        assert(vec.front() == std::string{"abc"});
        assert(vec.back() == std::string{"def"});
    }
    
    //pop_back() before_erase returns true
    {
        ovec_t vec;
        bool called{false};
        bool before_called{false};
        bool on_change_called{false};
        vec.emplace_back("abc");
        vec.before_erase([&before_called]
                         (const ovec_t&,
                          ovec_t::const_iterator it)
                         {
                             before_called = true;
                             assert(*it == std::string{"abc"});
                             return true;
                         });
        vec.after_erase([&called]
                        (const ovec_t&,
                         ovec_t::const_iterator)
                        {
                            called = true;
                        });
        vec.on_change([&on_change_called]
                      (const ovec_t&)
                      { on_change_called = true; });
        vec.pop_back();
        assert(before_called);
        assert(called);
        assert(on_change_called);
        assert(vec.empty());
    }
    
    //pop_back() before_erase returns false
    {
        ovec_t vec;
        bool called{false};
        bool before_called{false};
        bool on_change_called{false};
        vec.emplace_back("abc");
        vec.before_erase([&before_called]
                         (const ovec_t&,
                          ovec_t::const_iterator it)
                         {
                             before_called = true;
                             assert(*it == std::string{"abc"});
                             return false;
                         });
        vec.after_erase([&called]
                        (const ovec_t&,
                         ovec_t::const_iterator)
                        {
                            called = true;
                        });
        vec.pop_back();
        assert(before_called);
        assert(!called);
        assert(!on_change_called);
        assert(!vec.empty());
    }   
}
