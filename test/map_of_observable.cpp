#include "observable/map.hpp"
#include "observable/value.hpp"

#include <array>
#include <string>

using obs_t = observable::map<
    std::size_t, observable::value<std::string>
>;

int main()
{
    obs_t obs;

    //at fail
    {
        bool ok{false};
        try
        {
            obs.at(0);
        }
        catch(const std::out_of_range&)
        { ok = true; }
        assert(ok);
    }

    //at success
    {
        bool ok{true};
        obs.clear();
        obs.emplace(2, "abc");
        try
        {
            obs.at(2);
            bool called{false};
            auto& ob = obs.at(2);
            assert(ob.get() == "abc");
            ob.on_change([&called](std::string)
                          { called = true; });
            ob.assign("def");
            assert(ob.get() == "def");
            assert(called);
        }
        catch(const std::out_of_range&)
        { ok = false; }
        assert(ok);
    }
    
    //at const fail
    {
        bool ok{false};
        try
        {
            const auto& co = obs;
            co.at(0);
        }
        catch(const std::out_of_range&)
        { ok = true; }
        assert(ok);
    }

    //at const success
    {
        bool ok{true};
        obs.clear();
        obs.emplace(2, "abc");
        try
        {
            const auto& co = obs;
            auto& v = co.at(2);
            assert(v.get() == "abc");
        }
        catch(const std::out_of_range&)
        { ok = false; }
        assert(ok);            
    }

    //operator[] lvalue insert
    {
        bool called{false};
        obs_t::key_type k = 0;
        auto& ob = obs[k];
        // assert(ob == obs_t::mapped_type{});
        ob.on_change([&called](std::string)
                     { called = true; });
        ob = "def";
        assert(ob.get() == "def");
        assert(called);
    }

    //operator[] lvalue lookup
    {
        obs.clear();
        obs.emplace(2, "abc");
        obs_t::key_type k = 2;
        auto& ob = obs[k];
        assert(ob.get() == "abc");
        bool called{false};
        ob.on_change([&called](std::string)
                     { called = true; });
        ob = "def";
        assert(ob.get() == "def");
        assert(called);
    }
    
    // //operator[] rvalue insert
    // {
    //     auto& ob = obs[1];
    //     assert(ob == obs_t::mapped_type{});
    // }

    //operator[] rvalue lookup
    {
        obs.clear();
        obs.emplace(2, "abc");
        auto& ob = obs[2];
        assert(ob.get() == "abc");
        bool called{false};
        ob.on_change([&called](std::string)
                     { called = true; });
        ob = "def";
        assert(ob.get() == "def");
        assert(called);
    }

    //begin()
    {
        obs.clear();
        obs.emplace(2, "abc");
        auto& ob = obs.begin()->second;
        assert(ob.get() == "abc");
        bool called{false};
        ob.on_change([&called](std::string)
                     { called = true; });
        ob = "def";
        assert(ob.get() == "def");
        assert(called);
    }

    //cbegin()
    {
        obs.clear();
        obs.emplace(2, "abc");
        auto& ob = obs.cbegin()->second;
        assert(ob.get() == "abc");
    }
    
    //end/cend()
    {
        obs.clear();
        obs.emplace(2, "abc");
        assert(obs.begin() != obs.end());
        assert(obs.cbegin() != obs.cend());
    }

    //empty()
    {
        obs.clear();
        assert(obs.empty());
    }
    
    //size()
    {
        obs.clear();
        obs.emplace(2, "abc");
        obs.emplace(3, "def");
        assert(obs.size() == 2);
    }
    
    //max_size()
    {
        obs.clear();
        obs.emplace(2, "abc");
        obs.emplace(3, "def");
        assert(obs.max_size() >= 2);
    }
    
     //clear()
    {
        obs.clear();
        obs.emplace(2, "abc");
        obs.emplace(3, "def");
        bool called{false};
        bool before_called{false};
        boost::signals2::scoped_connection c1 =
            obs.before_erase([&before_called, &obs](obs_t::const_iterator it)
                         {
                             before_called = true;
                             assert(obs.end() == it);
                         });
        boost::signals2::scoped_connection c2 =
        obs.on_erase([&called](obs_t::const_iterator)
                     { called = true; });
        obs.clear();
        assert(before_called);
        assert(called);
        assert(obs.empty());
    }
    
     //erase()
    {
        obs.clear();
        obs.emplace(2, "abc");
        obs.emplace(3, "def");
        bool called{false};
        bool before_called{false};
        boost::signals2::scoped_connection c1 =
        obs.before_erase([&before_called](obs_t::const_iterator it)
                         {
                             before_called = true;
                             assert("abc" == it->second.get());
                         });
        boost::signals2::scoped_connection c2 =
        obs.on_erase([&called](obs_t::const_iterator)
                     { called = true; });
        obs.erase(obs.cbegin());
        assert(before_called);
        assert(called);
        assert(obs.size() == 1);
    }
    
     //erase(first, last)
    {
        obs.clear();
        obs.emplace(2, "abc");
        obs.emplace(3, "def");
        bool called{false};
        bool before_called{false};
        boost::signals2::scoped_connection c1 =
            obs.before_erase([&before_called, &obs](obs_t::const_iterator it)
                         {
                             before_called = true;
                             assert(obs.begin() == it);
                         });
        boost::signals2::scoped_connection c2 =
        obs.on_erase([&called](obs_t::const_iterator)
                     { called = true; });
        obs.erase(obs.cbegin(), obs.cend());
        assert(before_called);
        assert(called);
        assert(obs.empty());
    }
    
     //erase(key)
    {
        obs.clear();
        obs.emplace(2, "abc");
        obs.emplace(3, "def");
        bool called{false};
        bool before_called{false};
        boost::signals2::scoped_connection c1 =
        obs.before_erase([&before_called](obs_t::const_iterator it)
                         {
                             before_called = true;
                             assert("abc" == it->second.get());
                         });
        boost::signals2::scoped_connection c2 =
        obs.on_erase([&called](obs_t::const_iterator)
                                { called = true; });
        obs.erase(2);
        assert(called);
        assert(obs.cbegin()->second.get() == "def");
    }
    
    //emplace()
    {
        obs.clear();
        bool called{false};
        obs.on_insert([&called](obs_t::const_iterator)
                                 { called = true; });
        obs.emplace(2, "abc");
        obs.emplace(3, "def");
        assert(called);
        assert(obs.cbegin()->second.get() == "abc");
        assert(std::next(obs.cbegin())->second.get() == "def");
    }
    
    //emplace_hint()
    {
        obs.clear();
        bool called{false};
        obs.on_insert([&called](obs_t::const_iterator)
                                 { called = true; });
        obs.emplace_hint(obs.cbegin(), 2, "abc");
        obs.emplace_hint(obs.cbegin(),3, "def");
        assert(called);
        assert(obs.cbegin()->second.get() == "abc");
        assert(std::next(obs.cbegin())->second.get() == "def");
    }
    
    // //insert(const value_type&)
    // {
    //     obs.clear();
    //     bool called{false};
    //     obs.on_insert([&called](obs_t::const_iterator)
    //                              { called = true; });
    //     auto v = obs_t::value_type(2, "abc");
    //     obs.insert(v);
    //     auto v2 = obs_t::value_type(3, "def");
    //     obs.insert(v2);
    //     assert(called);
    //     assert(obs.cbegin()->second.get() == "abc");
    //     assert(std::next(obs.cbegin())->second.get() == "def");
    // }

    //insert(value_type&&)
    {
        obs.clear();
        bool called{false};
        obs.on_insert([&called](obs_t::const_iterator)
                                 { called = true; });
        obs.insert(obs_t::value_type(2, "abc"));
        obs.insert(obs_t::value_type(3, "def"));
        assert(called);
        assert(obs.cbegin()->second.get() == "abc");
        assert(std::next(obs.cbegin())->second.get() == "def");
    }
    
    //insert(hint, value_type&&)
    {
        obs.clear();
        bool called{false};
        obs.on_insert([&called](obs_t::const_iterator)
                                 { called = true; });
        obs.insert(obs.cbegin(), obs_t::value_type(2, "abc"));
        obs.insert(obs.cbegin(), obs_t::value_type(3, "def"));
        assert(called);
        assert(obs.cbegin()->second.get() == "abc");
        assert(std::next(obs.cbegin())->second.get() == "def");
    }
    
    //insert(first, last)
    {
        obs.clear();
        bool called{false};
        obs.on_insert([&called](obs_t::const_iterator)
                                 { called = true; });
        std::array<std::pair<std::size_t, std::string>, 2> a{{ {2, "abc"}, {3, "def"} }};
        obs.insert(a.cbegin(), a.cend());
        assert(called);
        assert(obs.cbegin()->second.get() == "abc");
        assert(std::next(obs.cbegin())->second.get() == "def");
    }

    // //insert(ilist)
    // {
    //     obs.clear();
    //     bool called{false};
    //     obs.on_insert([&called](obs_t::const_iterator)
    //                              { called = true; });
    //     obs.insert({ {2, std::string{"abc"}}, {3, std::string{"def"}} });
    //     assert(called);
    //     assert(obs.cbegin()->second.get() == "abc");
    //     assert(std::next(obs.cbegin())->second.get() == "def");
    // }

    // //swap
    // {
    //     obs.clear();
    //     bool ok{false};
    //     boost::signals2::scoped_connection c =
    //         obs.on_insert(
    //             [&ok](const obs_t&, obs_t::const_iterator)
    //             {
    //                 ok = true;
    //             });
    //     obs_t other{{2, "abc"}, {3, "def"}};
    //     obs.swap(other);
    //     assert(ok);
    // }
    
    //count
    {
        obs.clear();
        obs.emplace(2, "abc");
        obs.emplace(3, "def");
        assert(obs.count(2) == 1);
        assert(obs.count(0) == 0);
    }
    
    //find fail
    {
        obs.clear();
        assert(obs.find(0) == obs.end());
        const auto& cobs = obs;
        assert(cobs.find(0) == obs.cend());
        
    }
    
    //find success
    {
        obs.clear();
        obs.emplace(2, "abc");
        obs.emplace(3, "def");
        auto it = obs.find(2);
        assert(it != obs.end());
        auto& ob = *it;
        bool called{false};
        ob.second.on_change([&called](std::string)
                            { called = true; });
        auto it2 = obs.find(2);
        assert(it2 != obs.end());
        auto& ob2 = *it2;
        ob2.second = "def";
        assert(ob.second.get() == "def");
        assert(called);        
    }
    
    //equal_range
    {
        obs.clear();
        obs.emplace(2, "abc");
        obs.emplace(3, "def");
        assert(obs.equal_range(2).first != obs.end());
        assert(obs.equal_range(4).first == obs.end());
    }

    //crbegin/rbegin/crend/rend
    {
        obs.clear();
        obs.emplace(2, "abc");
        obs.emplace(3, "def");
        obs.emplace(4, "ghi");
        obs.emplace(5, "jkl");
        assert(obs.crbegin()->first == 5);
        assert(obs.rbegin()->first == 5);
        assert(std::prev(obs.crend())->first == 2);
        auto it = obs.rend();
        assert((--it)->first == 2);
    }
    
}
