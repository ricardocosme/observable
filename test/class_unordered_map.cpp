#include "observable/class.hpp"
#include "observable/unordered_map.hpp"

#include <array>
#include <iostream>
#include <string>
#include <unordered_map>

using map_t = std::unordered_map<std::size_t, std::string>;

struct foo_t
{
    map_t map;
};

struct map{};

using obs_t = observable::class_<
    foo_t,
    std::pair<map_t, map>
    >;

int main()
{
    foo_t foo;
    obs_t obs(foo, foo.map);

    //map_iterator default constructed
    {
        observable::observable_of_t<map_t>::iterator i;
    }
    
    //at fail
    {
        bool ok{false};
        try
        {
            obs.get<map>().at(0);
        }
        catch(const std::out_of_range&)
        { ok = true; }
        assert(ok);
    }

    //at success
    {
        bool ok{true};
        foo.map.clear();
        foo.map.emplace(2, "abc");
        try
        {
            bool called{false};
            auto ob = obs.get<map>().at(2);
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
            const auto& co = obs.get<map>();
            co.at(0);
        }
        catch(const std::out_of_range&)
        { ok = true; }
        assert(ok);
    }

    //at const success
    {
        bool ok{true};
        foo.map.clear();
        foo.map.emplace(2, "abc");
        try
        {
            const auto& co = obs.get<map>();
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
        auto ob = obs.get<map>()[k];
        assert(ob->get() == map_t::mapped_type{});
        ob->on_change([&called](const std::string&)
                     { called = true; });
        ob->assign("def");
        assert(ob->get() == "def");
        assert(called);
    }

    //operator[] lvalue lookup
    {
        foo.map.clear();
        foo.map.emplace(2, "abc");
        map_t::key_type k = 2;
        auto ob = obs.get<map>()[k];
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
        auto ob = obs.get<map>()[1];
        assert(ob->get() == map_t::mapped_type{});
    }

    //operator[] rvalue lookup
    {
        foo.map.clear();
        foo.map.emplace(2, "abc");
        auto ob = obs.get<map>()[2];
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
        foo.map.clear();
        foo.map.emplace(2, "abc");
        auto ob = obs.get<map>().begin()->second;
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
        foo.map.clear();
        foo.map.emplace(2, "abc");
        auto ob = obs.get<map>().cbegin()->second;
        assert(ob == "abc");
    }
    
    //end/cend()
    {
        foo.map.clear();
        foo.map.emplace(2, "abc");
        assert(obs.get<map>().begin() != obs.get<map>().end());
        assert(obs.get<map>().cbegin() != obs.get<map>().cend());
    }

    //empty()
    {
        foo.map.clear();
        assert(obs.get<map>().empty());
    }
    
    //size()
    {
        foo.map.clear();
        foo.map.emplace(2, "abc");
        foo.map.emplace(3, "def");
        assert(obs.get<map>().size() == 2);
    }
    
    //max_size()
    {
        foo.map.clear();
        foo.map.emplace(2, "abc");
        foo.map.emplace(3, "def");
        assert(obs.get<map>().max_size() >= 2);
    }
    
     //clear()
    {
        foo.map.clear();
        foo.map.emplace(2, "abc");
        foo.map.emplace(3, "def");
        bool called{false};
        bool before_called{false};
        boost::signals2::scoped_connection c1 =
            obs.get<map>().before_erase
            ([&before_called](const map_t& m, map_t::const_iterator it)
             {
                 before_called = true;
                 assert(m.end() == it);
             });
        boost::signals2::scoped_connection c2 =
            obs.get<map>().on_erase
            ([&called](const map_t&, map_t::value_type, map_t::const_iterator)
             { called = true; });
        obs.get<map>().clear();
        assert(before_called);
        assert(called);
        assert(obs.get<map>().empty());
    }
    
    //erase()
    {
        foo.map.clear();
        foo.map.emplace(2, "abc");
        foo.map.emplace(3, "def");
        bool called{false};
        bool before_called{false};
        boost::signals2::scoped_connection c1 =
            obs.get<map>().before_erase
            ([&before_called](const map_t& m, map_t::const_iterator it)
             {
                 before_called = true;
                 assert(it != m.end());
             });
        boost::signals2::scoped_connection c2 =
            obs.get<map>().on_erase
            ([&called](const map_t&, map_t::value_type, map_t::const_iterator)
             { called = true; });
        obs.get<map>().erase(obs.get<map>().cbegin());
        assert(before_called);
        assert(called);
        assert(obs.get<map>().size() == 1);
    }
    
     //erase(first, last)
    {
        foo.map.clear();
        foo.map.emplace(2, "abc");
        foo.map.emplace(3, "def");
        bool called{false};
        bool before_called{false};
        boost::signals2::scoped_connection c1 =
            obs.get<map>().before_erase
            ([&before_called](const map_t& m, map_t::const_iterator it)
             {
                 before_called = true;
                 assert(it != m.end());
             });
        boost::signals2::scoped_connection c2 =
            obs.get<map>().on_erase
            ([&called](const map_t&, map_t::value_type, map_t::const_iterator)
             { called = true; });
        obs.get<map>().erase(obs.get<map>().cbegin(), obs.get<map>().cend());
        assert(before_called);
        assert(called);
        assert(obs.get<map>().empty());
    }
    
     //erase(key)
    {
        foo.map.clear();
        foo.map.emplace(2, "abc");
        foo.map.emplace(3, "def");
        bool called{false};
        bool before_called{false};
        boost::signals2::scoped_connection c1 =
            obs.get<map>().before_erase
            ([&before_called](const map_t& m, map_t::const_iterator it)
             {
                 before_called = true;
                 assert(it->second == "abc");
             });
        boost::signals2::scoped_connection c2 =
            obs.get<map>().on_erase
            ([&called](const map_t&,
                       map_t::value_type v,
                       map_t::const_iterator)
             {
                 assert(v.second == "abc");
                 called = true;
             });
        obs.get<map>().erase(2);
        assert(called);
        auto crmap = obs.get<map>().get();
        assert(crmap.find(3)->second == "def");
    }
    
    //emplace()
    {
        foo.map.clear();
        bool called{false};
        obs.get<map>().on_insert([&called](const map_t&, map_t::const_iterator)
                                 { called = true; });
        obs.get<map>().emplace(2, "abc");
        obs.get<map>().emplace(3, "def");
        assert(called);
        auto crmap = obs.get<map>().get();
        assert(crmap.find(2)->second == "abc");
        assert(crmap.find(3)->second == "def");
    }
    
    //emplace_hint()
    {
        foo.map.clear();
        bool called{false};
        obs.get<map>().on_insert([&called](const map_t&, map_t::const_iterator)
                                 { called = true; });
        obs.get<map>().emplace_hint(foo.map.cbegin(), 2, "abc");
        obs.get<map>().emplace_hint(foo.map.cbegin(),3, "def");
        assert(called);
        auto crmap = obs.get<map>().get();
        assert(crmap.find(2)->second == "abc");
        assert(crmap.find(3)->second == "def");
    }
    
    //insert(const value_type&)
    {
        foo.map.clear();
        bool called{false};
        obs.get<map>().on_insert([&called](const map_t&, map_t::const_iterator)
                                 { called = true; });
        auto v = map_t::value_type(2, "abc");
        obs.get<map>().insert(v);
        auto v2 = map_t::value_type(3, "def");
        obs.get<map>().insert(v2);
        assert(called);
        auto crmap = obs.get<map>().get();
        assert(crmap.find(2)->second == "abc");
        assert(crmap.find(3)->second == "def");
    }

    //insert(value_type&&)
    {
        foo.map.clear();
        bool called{false};
        obs.get<map>().on_insert([&called](const map_t&, map_t::const_iterator)
                                 { called = true; });
        obs.get<map>().insert(map_t::value_type(2, "abc"));
        obs.get<map>().insert(map_t::value_type(3, "def"));
        assert(called);
        auto crmap = obs.get<map>().get();
        assert(crmap.find(2)->second == "abc");
        assert(crmap.find(3)->second == "def");
    }
    
    //insert(hint, value_type&&)
    {
        foo.map.clear();
        bool called{false};
        obs.get<map>().on_insert([&called](const map_t&, map_t::const_iterator)
                                 { called = true; });
        obs.get<map>().insert(foo.map.cbegin(), map_t::value_type(2, "abc"));
        obs.get<map>().insert(foo.map.cbegin(), map_t::value_type(3, "def"));
        assert(called);
        auto crmap = obs.get<map>().get();
        assert(crmap.find(2)->second == "abc");
        assert(crmap.find(3)->second == "def");
    }
    
    //insert(first, last)
    {
        foo.map.clear();
        bool called{false};
        obs.get<map>().on_insert([&called](const map_t&, map_t::const_iterator)
                                 { called = true; });
        std::array<std::pair<std::size_t, std::string>, 2> a{{ {2, "abc"}, {3, "def"} }};
        obs.get<map>().insert(a.cbegin(), a.cend());
        assert(called);
        auto crmap = obs.get<map>().get();
        assert(crmap.find(2)->second == "abc");
        assert(crmap.find(3)->second == "def");
    }

    //insert(ilist)
    {
        foo.map.clear();
        bool called{false};
        obs.get<map>().on_insert([&called](const map_t&, map_t::const_iterator)
                                 { called = true; });
        obs.get<map>().insert({ {2, "abc"}, {3, "def"} });
        assert(called);
        auto crmap = obs.get<map>().get();
        assert(crmap.find(2)->second == "abc");
        assert(crmap.find(3)->second == "def");
    }

    //swap
    {
        foo.map.clear();
        bool ok{false};
        boost::signals2::scoped_connection c =
            obs.get<map>().on_insert(
                [&ok](const map_t&, map_t::const_iterator)
                {
                    ok = true;
                });
        map_t other{{2, "abc"}, {3, "def"}};
        obs.get<map>().swap(other);
        assert(ok);
    }
    
    //count
    {
        foo.map.clear();
        obs.get<map>().insert({ {2, "abc"}, {3, "def"} });
        assert(obs.get<map>().count(2) == 1);
        assert(obs.get<map>().count(0) == 0);
    }
    
    //find fail
    {
        foo.map.clear();
        assert(obs.get<map>().find(0) == obs.get<map>().end());
        const auto& cobs = obs.get<map>();
        assert(cobs.find(0) == obs.get<map>().cend());
        
    }
    
    //find success
    {
        foo.map.clear();
        foo.map.emplace(2, "abc");
        foo.map.emplace(3, "def");
        auto it = obs.get<map>().find(2);
        assert(it != obs.get<map>().end());
        auto ob = *it;
        bool called{false};
        ob.second->on_change([&called](const std::string&)
                            { called = true; });
        auto it2 = obs.get<map>().find(2);
        assert(it2 != obs.get<map>().end());
        auto ob2 = *it2;
        ob2.second->assign("def");
        assert(ob.second->get() == "def");
        assert(called);        
    }
    
    //equal_range
    {
        foo.map.clear();
        foo.map.emplace(2, "abc");
        foo.map.emplace(3, "def");
        assert(obs.get<map>().equal_range(2).first != obs.get<map>().end());
        assert(obs.get<map>().equal_range(4).first == obs.get<map>().end());
    }

}
