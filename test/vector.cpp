#include <observable/vector.hpp>

#include <string>
#include <vector>

using namespace observable;

template<typename ocontainer, typename value_type>
void check_equal(ocontainer& c, std::initializer_list<value_type> il)
{
    auto it = il.begin();
    assert(c.size() == il.size());
    for(auto e : c)
        assert(e == *it++);
}

int main()
{
    {
        vector<std::string> vec;        
    }

    {
        vector<std::string> vec(5);
        assert(vec.size() == 5);
        for(auto e : vec)
            assert(e == std::string{});
    }
    
    {
        vector<std::string> vec(5, "something");
        assert(vec.size() == 5);
        for(auto e : vec)
            assert(e == std::string{"something"});
    }
    
    {
        vector<std::string> vec{"abc", "def", "ghi"};
        assert(vec.size() == 3);
        check_equal(vec, {"abc", "def", "ghi"});
    }
    
    {
        std::vector<std::string> src{"abc", "def", "ghi"};
        vector<std::string> vec(src.begin(), src.end());
        assert(vec.size() == 3);
        check_equal(vec, {"abc", "def", "ghi"});
    }
    
    //begin()
    {
        vector<std::string> vec;
        vec.emplace_back("abc");
        assert(*vec.begin() == "abc");
    }

    //cbegin()
    {
        vector<std::string> vec;
        vec.emplace_back("abc");
        assert(*vec.cbegin() == "abc");
    }
    
    //rbegin()
    {
        vector<std::string> vec;
        vec.emplace_back("abc");
        vec.emplace_back("def");
        assert(*vec.rbegin() == "def");
    }
    
    //crbegin()
    {
        vector<std::string> vec;
        vec.emplace_back("abc");
        vec.emplace_back("def");
        assert(*vec.crbegin() == "def");
    }
    
    //end()
    {
        vector<std::string> vec;
        vec.emplace_back("abc");
        vec.emplace_back("def");
        assert(std::distance(vec.begin(), vec.end()) == 2);
    }

    //cend()
    {
        vector<std::string> vec;
        vec.emplace_back("abc");
        vec.emplace_back("def");
        assert(std::distance(vec.cbegin(), vec.cend()) == 2);
    }    

    //empty()
    {
        vector<std::string> vec;
        assert(vec.empty());
        vec.emplace_back("something");
        assert(!vec.empty());        
    }
    
    //size()
    {
        vector<std::string> vec;
        vec.emplace_back("abc");
        vec.emplace_back("def");
        assert(vec.size() == 2);
    }
    
    //max_size()
    {
        vector<std::string> vec;
        vec.emplace_back("abc");
        vec.emplace_back("def");
        assert(vec.max_size() >= 2);
    }
    
    //reserve()
    {
        vector<std::string> vec;
        vec.reserve(100);
    }
    
    //capacity()
    {
        vector<std::string> vec;
        vec.emplace_back("abc");
        assert(vec.capacity() >= 1);
    }

    //shrink_to_fit()
    {
        vector<std::string> vec;
        vec.emplace_back("abc");
        vec.shrink_to_fit();
    }
    
    //at fail
    {
        vector<std::string> vec;
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
        vector<std::string> vec;
        vec.emplace(vec.begin(), "abc");
        try
        {
            assert(vec.at(0) == "abc");
            bool on_value_change_called{false};
            bool on_change_called{false};
            vec.on_value_change
                ([&on_value_change_called](const vector<std::string>&,
                                           vector<std::string>::const_iterator it)
                 {
                     assert(*it == "change");
                     on_value_change_called = true;
                 });
            vec.on_change
                ([&on_change_called](const vector<std::string>&)
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
        vector<std::string> vec;
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
        vector<std::string> vec;
        vec.emplace(vec.begin(), "abc");
        try
        {
            const auto& cvec = vec;
            auto cvalue = cvec.at(0);
            assert(cvalue == "abc");
        }
        catch(const std::out_of_range&)
        { ok = false; }
        assert(ok);            
    }

    //operator[] 
    {
        vector<std::string> vec;
        vec.emplace(vec.begin(), "abc");
        assert(vec[0] == "abc");
        bool on_value_change_called{false};
        bool on_change_called{false};
        vec.on_value_change
            ([&on_value_change_called](const vector<std::string>&,
                                       vector<std::string>::const_iterator it)
             {
                 assert(*it == "change");
                 on_value_change_called = true;
             });
        vec.on_change
            ([&on_change_called](const vector<std::string>&)
             {
                 on_change_called = true;
             });
        vec[0] = "change";
        assert(on_value_change_called);
        assert(on_change_called);
    }

    //operator[] const
    {
        vector<std::string> vec;
        vec.emplace(vec.begin(), "abc");
        const auto& cvec = vec;
        assert(cvec[0] == "abc");
    }
    
    //front()
    {
        vector<std::string> vec;
        vec.emplace(vec.begin(), "abc");
        assert(vec.front() == "abc");
        bool on_value_change_called{false};
        bool on_change_called{false};
        vec.on_value_change
            ([&on_value_change_called](const vector<std::string>&,
                                       vector<std::string>::const_iterator it)
             {
                 assert(*it == "change");
                 on_value_change_called = true;
             });
        vec.on_change
            ([&on_change_called](const vector<std::string>&)
             {
                 on_change_called = true;
             });
        vec.front() = "change";
        assert(on_value_change_called);
        assert(on_change_called);
    }
    
    //front() const
    {
        vector<std::string> vec;
        vec.emplace(vec.begin(), "abc");
        auto& cvec = vec;
        assert(cvec.front() == "abc");
    }
    
    //back()
    {
        vector<std::string> vec;
        vec.emplace_back("abc");
        vec.emplace_back("def");
        assert(vec.back() == "def");
        bool on_value_change_called{false};
        bool on_change_called{false};
        vec.on_value_change
            ([&on_value_change_called](const vector<std::string>&,
                                       vector<std::string>::const_iterator it)
             {
                 assert(*it == "change");
                 on_value_change_called = true;
             });
        vec.on_change
            ([&on_change_called](const vector<std::string>&)
             {
                 on_change_called = true;
             });
        vec.back() = "change";
        assert(on_value_change_called);
        assert(on_change_called);
    }
    
    //back() const
    {
        vector<std::string> vec;
        vec.emplace_back("abc");
        vec.emplace_back("def");
        const auto& cvec = vec;
        assert(cvec.back() == "def");
    }
        
    //clear() before_erase returns true
    {
        vector<std::string> vec;
        vec.emplace_back("abc");
        vec.emplace_back("def");
        bool after_called{false};
        bool before_called{false};
        bool on_change_called{false};
        vec.before_erase([&before_called]
                         (const vector<std::string>& vec,
                          vector<std::string>::const_iterator it)
                         {
                             before_called = true;
                             assert(vec.cend() == it);
                             return true;
                         });
        vec.after_erase([&after_called]
                        (const vector<std::string>&,
                         vector<std::string>::const_iterator)
                        { after_called = true; });
        vec.on_change([&on_change_called]
                      (const vector<std::string>&)
                      { on_change_called = true; });
        assert(vec.clear());
        assert(before_called);
        assert(after_called);
        assert(on_change_called);
    }
            
    //clear() before_erase returns false
    {
        vector<std::string> vec;
        vec.emplace_back("abc");
        vec.emplace_back("def");
        bool after_called{false};
        bool before_called{false};
        bool on_change_called{false};
        vec.before_erase([&before_called]
                         (const vector<std::string>& vec,
                          vector<std::string>::const_iterator it)
                         {
                             before_called = true;
                             assert(vec.cend() == it);
                             return false;
                         });
        vec.after_erase([&after_called]
                        (const vector<std::string>&,
                         vector<std::string>::const_iterator)
                        { after_called = true; });
        vec.on_change([&on_change_called]
                      (const vector<std::string>&)
                      { on_change_called = true; });
        assert(!vec.clear());
        assert(before_called);
        assert(!after_called);
        assert(!on_change_called);
    }
    
    //erase() before_erase returns true
    {
        vector<std::string> vec;
        vec.emplace_back("abc");
        vec.emplace_back("def");
        bool after_called{false};
        bool before_called{false};
        bool on_change_called{false};
        vec.before_erase([&before_called]
                         (const vector<std::string>& vec,
                          vector<std::string>::const_iterator it)
                         {
                             before_called = true;
                             assert(*it == "abc");
                             return true;
                         });
        vec.after_erase([&after_called]
                        (const vector<std::string>&,
                         vector<std::string>::const_iterator)
                        { after_called = true; });
        vec.on_change([&on_change_called]
                      (const vector<std::string>&)
                      { on_change_called = true; });
        auto ret = vec.erase(vec.begin());
        assert(ret.second);
        assert(*ret.first == "def");
        assert(vec.size() == 1);
        assert(before_called);
        assert(after_called);
        assert(on_change_called);
    }
    
    //erase() before_erase returns false
    {
        vector<std::string> vec;
        vec.emplace_back("abc");
        vec.emplace_back("def");
        bool after_called{false};
        bool before_called{false};
        bool on_change_called{false};
        vec.before_erase([&before_called]
                         (const vector<std::string>& vec,
                          vector<std::string>::const_iterator it)
                         {
                             before_called = true;
                             assert(*it == "abc");
                             return false;
                         });
        vec.after_erase([&after_called]
                        (const vector<std::string>&,
                         vector<std::string>::const_iterator)
                        { after_called = true; });
        vec.on_change([&on_change_called]
                      (const vector<std::string>&)
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
        vector<std::string> vec;
        vec.emplace_back("abc");
        vec.emplace_back("def");
        bool called{false};
        bool before_called{false};
        bool on_change_called{false};
        vec.before_erase([&before_called]
                          (const vector<std::string>& vec,
                           vector<std::string>::const_iterator it)
                         {
                             before_called = true;
                             assert(vec.cbegin() == it);
                             return true;
                         });
        vec.after_erase([&called](const vector<std::string>&,
                                  vector<std::string>::const_iterator)
                        { called = true; });
        vec.on_change([&on_change_called]
                      (const vector<std::string>&)
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
        vector<std::string> vec;
        vec.emplace_back("abc");
        vec.emplace_back("def");
        bool called{false};
        bool before_called{false};
        bool on_change_called{false};
        vec.before_erase([&before_called]
                          (const vector<std::string>& vec,
                           vector<std::string>::const_iterator it)
                         {
                             before_called = true;
                             assert(vec.cbegin() == it);
                             return false;
                         });
        vec.after_erase([&called](const vector<std::string>&,
                                  vector<std::string>::const_iterator)
                        { called = true; });
        vec.on_change([&on_change_called]
                      (const vector<std::string>&)
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
        vector<std::string> vec;
        bool called{false};
        bool on_change_called{false};
        vec.emplace_back("abc");
        vec.after_insert([&called](const vector<std::string>&,
                                   vector<std::string>::const_iterator it)
                       {
                           assert(*it == "def");
                           called = true;
                       });
        vec.on_change([&on_change_called]
                      (const vector<std::string>&)
                      { on_change_called = true; });
        vec.emplace(vec.begin(), "def");
        assert(called);
        assert(on_change_called);
        assert(vec.front() == "def");
        assert(vec.size() == 2);
    }
    
    //insert(pos, const value_type&)
    {
        vector<std::string> vec;
        std::string v = "abc";
        vec.insert(vec.begin(), v);
        bool called{false};
        bool on_change_called{false};
        vec.after_insert([&called](const vector<std::string>&,
                                   vector<std::string>::const_iterator it)
                         {
                             assert(*it == "def");
                             called = true;
                         });
        vec.on_change([&on_change_called]
                      (const vector<std::string>&)
                      { on_change_called = true; });
        std::string v2 = "def";
        vec.insert(std::next(vec.begin()), v2);
        assert(called);
        assert(on_change_called);
        assert(vec.front() == "abc");
        assert(*std::next(vec.cbegin()) == "def");
        assert(vec.size() == 2);
    }

    //insert(pos, value_type&&)
    {
        vector<std::string> vec;
        bool called{false};
        bool on_change_called{false};
        std::string v = "abc";
        vec.insert(vec.begin(), v);
        vec.after_insert([&called](const vector<std::string>&,
                                   vector<std::string>::const_iterator it)
                         {
                             assert(*it == "def");
                             called = true;
                         });
        vec.on_change([&on_change_called]
                      (const vector<std::string>&)
                      { on_change_called = true; });
        std::string v2 = "def";
        vec.insert(std::next(vec.begin()), v2);
        assert(called);
        assert(on_change_called);
        assert(vec.front() == "abc");
        assert(*std::next(vec.cbegin()) == "def");
        assert(vec.size() == 2);
    }
    
    //insert(pos, count, const value_type&)
    {
        vector<std::string> vec;
        bool called{false};
        bool on_change_called{false};
        vec.after_insert([&called](const vector<std::string>&,
                                   vector<std::string>::const_iterator it)
                         {
                             assert(*it == "abc");
                             called = true;
                         });
        vec.on_change([&on_change_called]
                      (const vector<std::string>&)
                      { on_change_called = true; });
        auto it = vec.begin();
        vec.insert(it, 2, "abc");
        assert(called);
        assert(on_change_called);
        assert(*std::next(vec.cbegin()) == "abc");
        assert(vec.size() == 2);
    }
    
    //insert(pos, first, last)
    {
        vector<std::string> vec;
        bool called{false};
        bool on_change_called{false};
        vec.after_insert([&called](const vector<std::string>&,
                                   vector<std::string>::const_iterator it)
                         {
                             assert(*it == "abc");
                             called = true;
                         });
        vec.on_change([&on_change_called]
                      (const vector<std::string>&)
                      { on_change_called = true; });
        auto it = vec.begin();
        std::initializer_list<std::string> src{"abc", "def"};
        vec.insert(it, src.begin(), src.end());
        assert(called);
        assert(on_change_called);
        assert(*std::next(vec.cbegin()) == "def");
        assert(vec.size() == 2);
    }

    //insert(ilist)
    {
        vector<std::string> vec;
        bool called{false};
        bool on_change_called{false};
        vec.after_insert([&called](const vector<std::string>&,
                                   vector<std::string>::const_iterator it)
                         {
                             assert(*it == "abc");
                             called = true;
                         });
        vec.on_change([&on_change_called]
                      (const vector<std::string>&)
                      { on_change_called = true; });
        auto it = vec.begin();
        vec.insert(it, {"abc", "def"});
        assert(called);
        assert(on_change_called);
        assert(*std::next(vec.cbegin()) == "def");
        assert(vec.size() == 2);
    }
    
    //push_back()
    {
        vector<std::string> vec;
        bool called{false};
        bool on_change_called{false};
        vec.emplace_back("abc");
        boost::signals2::scoped_connection c =
            vec.after_insert([&called]
                           (const vector<std::string>&,
                            vector<std::string>::const_iterator it)
                           {
                               assert(*it == "def");
                               called = true;
                           });
        vec.on_change([&on_change_called]
                      (const vector<std::string>&)
                      { on_change_called = true; });
        vec.push_back(std::string{"def"});
        auto lvalue = std::string{"def"};
        vec.push_back(lvalue);
        assert(called);
        assert(on_change_called);
        assert(vec.front() == "abc");
        assert(vec.back() == "def");
    }
    
    //emplace_back()
    {
        vector<std::string> vec;
        bool called{false};
        bool on_change_called{false};
        vec.emplace_back("abc");
        boost::signals2::scoped_connection c =
            vec.after_insert([&called]
                           (const vector<std::string>&,
                            vector<std::string>::const_iterator it)
                           {
                               assert(*it == "def");
                               called = true;
                           });
        vec.on_change([&on_change_called]
                      (const vector<std::string>&)
                      { on_change_called = true; });
        vec.emplace_back("def");
        assert(called);
        assert(on_change_called);
        assert(vec.front() == "abc");
        assert(vec.back() == "def");
    }
    
    //pop_back() before_erase returns true
    {
        vector<std::string> vec;
        bool called{false};
        bool before_called{false};
        bool on_change_called{false};
        vec.emplace_back("abc");
        vec.before_erase([&before_called]
                         (const vector<std::string>&,
                          vector<std::string>::const_iterator it)
                         {
                             before_called = true;
                             assert(*it == "abc");
                             return true;
                         });
        vec.after_erase([&called]
                        (const vector<std::string>&,
                         vector<std::string>::const_iterator)
                        {
                            called = true;
                        });
        vec.on_change([&on_change_called]
                      (const vector<std::string>&)
                      { on_change_called = true; });
        vec.pop_back();
        assert(before_called);
        assert(called);
        assert(on_change_called);
        assert(vec.empty());
    }
    
    //pop_back() before_erase returns false
    {
        vector<std::string> vec;
        bool called{false};
        bool before_called{false};
        bool on_change_called{false};
        vec.emplace_back("abc");
        vec.before_erase([&before_called]
                         (const vector<std::string>&,
                          vector<std::string>::const_iterator it)
                         {
                             before_called = true;
                             assert(*it == "abc");
                             return false;
                         });
        vec.after_erase([&called]
                        (const vector<std::string>&,
                         vector<std::string>::const_iterator)
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
