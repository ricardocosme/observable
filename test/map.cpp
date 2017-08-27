#include "observable/map.hpp"

#include <array>
#include <iostream>
#include <string>
#include <unordered_set>

using map_t = std::map<std::size_t, std::string>;

using obs_t = observable::map<map_t>;

int main()
{
    map_t map;
    obs_t obs(map);

    //map_iterator default constructed
    {
        observable::observable_of_t<map_t>::iterator i;
    }
    
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
        map.clear();
        map.emplace(2, "abc");
        try
        {
            bool called{false};
            auto ob = obs.at(2);
            assert(ob->get() == "abc");
            ob->on_change([&called](const std::string&)
                          { called = true; });
            ob->assign("def");
            assert(ob->get() == "def");
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
        map.clear();
        map.emplace(2, "abc");
        try
        {
            const auto& co = obs;
            auto v = co.at(2);
            assert(v == "abc");
        }
        catch(const std::out_of_range&)
        { ok = false; }
        assert(ok);            
    }

    //operator[] lvalue insert
    {
        bool called{false};
        map_t::key_type k = 0;
        auto ob = obs[k];
        assert(ob->get() == map_t::mapped_type{});
        ob->on_change([&called](const std::string&)
                     { called = true; });
        ob->assign("def");
        assert(ob->get() == "def");
        assert(called);
    }

    //operator[] lvalue lookup
    {
        map.clear();
        map.emplace(2, "abc");
        map_t::key_type k = 2;
        auto ob = obs[k];
        assert(ob->get() == "abc");
        bool called{false};
        ob->on_change([&called](const std::string&)
                     { called = true; });
        ob->assign("def");
        assert(ob->get() == "def");
        assert(called);
    }
    
    //operator[] rvalue insert
    {
        auto ob = obs[1];
        assert(ob->get() == map_t::mapped_type{});
    }

    //operator[] rvalue lookup
    {
        map.clear();
        map.emplace(2, "abc");
        auto ob = obs[2];
        assert(ob->get() == "abc");
        bool called{false};
        ob->on_change([&called](const std::string&)
                     { called = true; });
        ob->assign("def");
        assert(ob->get() == "def");
        assert(called);
    }

    //begin()
    {
        map.clear();
        map.emplace(2, "abc");
        auto ob = obs.begin()->second;
        assert(ob->get() == "abc");
        bool called{false};
        ob->on_change([&called](const std::string&)
                     { called = true; });
        ob->assign("def");
        assert(ob->get() == "def");
        assert(called);
    }

    //cbegin()
    {
        map.clear();
        map.emplace(2, "abc");
        auto ob = obs.cbegin()->second;
        assert(ob == "abc");
    }
    
    //end/cend()
    {
        map.clear();
        map.emplace(2, "abc");
        assert(obs.begin() != obs.end());
        assert(obs.cbegin() != obs.cend());
    }

    //empty()
    {
        map.clear();
        assert(obs.empty());
    }
    
    //size()
    {
        map.clear();
        map.emplace(2, "abc");
        map.emplace(3, "def");
        assert(obs.size() == 2);
    }
    
    //max_size()
    {
        map.clear();
        map.emplace(2, "abc");
        map.emplace(3, "def");
        assert(obs.max_size() >= 2);
    }
    
     //clear()
    {
        map.clear();
        map.emplace(2, "abc");
        map.emplace(3, "def");
        bool called{false};
        bool before_called{false};
        boost::signals2::scoped_connection c1 =
        obs.before_erase([&before_called](const map_t& c,
                                          map_t::const_iterator it)
                         {
                             before_called = true;
                             assert(c.end() == it);
                         });
        boost::signals2::scoped_connection c2 =
        obs.on_erase([&called](const map_t&,
                               map_t::const_iterator)
                     { called = true; });
        obs.clear();
        assert(before_called);
        assert(called);
        assert(obs.empty());
    }
    
     //erase()
    {
        map.clear();
        map.emplace(2, "abc");
        map.emplace(3, "def");
        bool called{false};
        bool before_called{false};
        boost::signals2::scoped_connection c1 =
        obs.before_erase([&before_called](const map_t& c,
                                          map_t::const_iterator it)
                         {
                             before_called = true;
                             assert("abc" == it->second);
                         });
        boost::signals2::scoped_connection c2 =
        obs.on_erase([&called](const map_t&,
                               map_t::const_iterator)
                     { called = true; });
        obs.erase(obs.cbegin());
        assert(before_called);
        assert(called);
        assert(obs.size() == 1);
    }
    
     //erase(first, last)
    {
        map.clear();
        map.emplace(2, "abc");
        map.emplace(3, "def");
        bool called{false};
        bool before_called{false};
        boost::signals2::scoped_connection c1 =
        obs.before_erase([&before_called](const map_t& c,
                                          map_t::const_iterator it)
                         {
                             before_called = true;
                             assert(c.begin() == it);
                         });
        boost::signals2::scoped_connection c2 =
        obs.on_erase([&called](const map_t&,
                               map_t::const_iterator)
                     { called = true; });
        obs.erase(obs.cbegin(), obs.cend());
        assert(before_called);
        assert(called);
        assert(obs.empty());
    }
    
     //erase(key)
    {
        map.clear();
        map.emplace(2, "abc");
        map.emplace(3, "def");
        bool called{false};
        bool before_called{false};
        boost::signals2::scoped_connection c1 =
        obs.before_erase([&before_called](const map_t& c,
                                          map_t::const_iterator it)
                         {
                             before_called = true;
                             assert("abc" == it->second);
                         });
        boost::signals2::scoped_connection c2 =
        obs.on_erase([&called](const map_t&, map_t::const_iterator)
                                { called = true; });
        obs.erase(2);
        assert(called);
        assert(obs.cbegin()->second == "def");
    }
    
    //emplace()
    {
        map.clear();
        bool called{false};
        obs.on_insert([&called](const map_t&, map_t::const_iterator)
                                 { called = true; });
        obs.emplace(2, "abc");
        obs.emplace(3, "def");
        assert(called);
        assert(obs.cbegin()->second == "abc");
        assert(std::next(obs.cbegin())->second == "def");
    }
    
    //emplace_hint()
    {
        map.clear();
        bool called{false};
        obs.on_insert([&called](const map_t&, map_t::const_iterator)
                                 { called = true; });
        obs.emplace_hint(map.cbegin(), 2, "abc");
        obs.emplace_hint(map.cbegin(),3, "def");
        assert(called);
        assert(obs.cbegin()->second == "abc");
        assert(std::next(obs.cbegin())->second == "def");
    }
    
    //insert(const value_type&)
    {
        map.clear();
        bool called{false};
        obs.on_insert([&called](const map_t&, map_t::const_iterator)
                                 { called = true; });
        auto v = map_t::value_type(2, "abc");
        obs.insert(v);
        auto v2 = map_t::value_type(3, "def");
        obs.insert(v2);
        assert(called);
        assert(obs.cbegin()->second == "abc");
        assert(std::next(obs.cbegin())->second == "def");
    }

    //insert(value_type&&)
    {
        map.clear();
        bool called{false};
        obs.on_insert([&called](const map_t&, map_t::const_iterator)
                                 { called = true; });
        obs.insert(map_t::value_type(2, "abc"));
        obs.insert(map_t::value_type(3, "def"));
        assert(called);
        assert(obs.cbegin()->second == "abc");
        assert(std::next(obs.cbegin())->second == "def");
    }
    
    //insert(hint, value_type&&)
    {
        map.clear();
        bool called{false};
        obs.on_insert([&called](const map_t&, map_t::const_iterator)
                                 { called = true; });
        obs.insert(map.cbegin(), map_t::value_type(2, "abc"));
        obs.insert(map.cbegin(), map_t::value_type(3, "def"));
        assert(called);
        assert(obs.cbegin()->second == "abc");
        assert(std::next(obs.cbegin())->second == "def");
    }
    
    //insert(first, last)
    {
        map.clear();
        bool called{false};
        obs.on_insert([&called](const map_t&, map_t::const_iterator)
                                 { called = true; });
        std::array<std::pair<std::size_t, std::string>, 2> a{{ {2, "abc"}, {3, "def"} }};
        obs.insert(a.cbegin(), a.cend());
        assert(called);
        assert(obs.cbegin()->second == "abc");
        assert(std::next(obs.cbegin())->second == "def");
    }

    //insert(ilist)
    {
        map.clear();
        bool called{false};
        obs.on_insert([&called](const map_t&, map_t::const_iterator)
                                 { called = true; });
        obs.insert({ {2, "abc"}, {3, "def"} });
        assert(called);
        assert(obs.cbegin()->second == "abc");
        assert(std::next(obs.cbegin())->second == "def");
    }

    //swap
    {
        map.clear();
        bool ok{false};
        boost::signals2::scoped_connection c =
            obs.on_insert(
                [&ok](const map_t&, map_t::const_iterator)
                {
                    ok = true;
                });
        map_t other{{2, "abc"}, {3, "def"}};
        obs.swap(other);
        assert(ok);
    }
    
    //count
    {
        map.clear();
        obs.insert({ {2, "abc"}, {3, "def"} });
        assert(obs.count(2) == 1);
        assert(obs.count(0) == 0);
    }
    
    //find fail
    {
        map.clear();
        assert(obs.find(0) == obs.end());
        const auto& cobs = obs;
        assert(cobs.find(0) == obs.cend());
        
    }
    
    //find success
    {
        map.clear();
        map.emplace(2, "abc");
        map.emplace(3, "def");
        auto it = obs.find(2);
        assert(it != obs.end());
        auto ob = *it;
        bool called{false};
        ob.second->on_change([&called](const std::string&)
                            { called = true; });
        auto it2 = obs.find(2);
        assert(it2 != obs.end());
        auto ob2 = *it2;
        ob2.second->assign("def");
        assert(ob.second->get() == "def");
        assert(called);        
    }
    
    //equal_range
    {
        map.clear();
        map.emplace(2, "abc");
        map.emplace(3, "def");
        assert(obs.equal_range(2).first != obs.end());
        assert(obs.equal_range(4).first == obs.end());
    }

    //crbegin/rbegin/crend/rend
    {
        map.clear();
        map.emplace(2, "abc");
        map.emplace(3, "def");
        map.emplace(4, "ghi");
        map.emplace(5, "jkl");
        auto& omap = obs;
        assert(omap.crbegin()->first == 5);
        assert(omap.rbegin()->first == 5);
        assert(std::prev(omap.crend())->first == 2);
        auto it = omap.rend();
        assert((--it)->first == 2);
    }
    
}
