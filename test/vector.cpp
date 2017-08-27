#include "observable/class.hpp"

#include <array>
#include <iostream>
#include <string>
#include <vector>

using vector_t = std::vector<std::string>;

struct foo_t
{
    vector_t vector;
};

struct vector{};

using obs_t = observable::class_<
    foo_t,
    std::pair<vector_t, vector>
    >;

int main()
{
    foo_t foo;
    obs_t obs(foo, foo.vector);

    //vector_iterator default constructed
    {
        observable::observable_of_t<vector_t>::iterator i;
    }
    
    //at fail
    {
        bool ok{false};
        try
        {
            obs.get<vector>().at(0);
        }
        catch(const std::out_of_range&)
        { ok = true; }
        assert(ok);
    }

    //at success
    {
        bool ok{true};
        foo.vector.clear();
        foo.vector.emplace(foo.vector.begin(), "abc");
        try
        {
            bool called{false};
            auto ob = obs.get<vector>().at(0);
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
        foo.vector.clear();
        bool ok{false};
        try
        {
            const auto& co = obs.get<vector>();
            co.at(0);
        }
        catch(const std::out_of_range&)
        { ok = true; }
        assert(ok);
    }

    //at const success
    {
        bool ok{true};
        foo.vector.clear();
        foo.vector.emplace(foo.vector.begin(), "abc");
        try
        {
            const auto& co = obs.get<vector>();
            auto v = co.at(0);
            assert(v == "abc");
        }
        catch(const std::out_of_range&)
        { ok = false; }
        assert(ok);            
    }

    //operator[] 
    {
        foo.vector.clear();
        foo.vector.emplace(foo.vector.begin(), "abc");
        bool called{false};
        auto ob = obs.get<vector>()[0];
        assert(ob->get() == "abc");
        ob->on_change([&called](const std::string&)
                     { called = true; });
        ob->assign("def");
        assert(ob->get() == "def");
        assert(called);
    }

    //operator[] const
    {
        foo.vector.clear();
        foo.vector.emplace(foo.vector.begin(), "abc");
        const auto& cobs = obs.get<vector>();
        assert(cobs[0] == "abc");
    }
    
    //front()
    {
        foo.vector.clear();
        foo.vector.emplace(foo.vector.begin(), "abc");
        auto ob = obs.get<vector>().front();
        assert(ob->get() == "abc");
        bool called{false};
        ob->on_change([&called](const std::string&)
                     { called = true; });
        ob->assign("def");
        assert(ob->get() == "def");
        assert(called);
    }
    
    //front() const
    {
        foo.vector.clear();
        foo.vector.emplace(foo.vector.begin(), "abc");
        auto& cobs = obs.get<vector>();
        auto ob = cobs.front();
        assert(ob->get() == "abc");
        bool called{false};
        ob->on_change([&called](const std::string&)
                     { called = true; });
        ob->assign("def");
        assert(ob->get() == "def");
        assert(called);
    }
    
    //back()
    {
        foo.vector.clear();
        foo.vector.emplace(foo.vector.begin(), "abc");
        foo.vector.emplace(std::next(foo.vector.begin()), "def");
        auto ob = obs.get<vector>().back();
        assert(ob->get() == "def");
        bool called{false};
        ob->on_change([&called](const std::string&)
                     { called = true; });
        ob->assign("ghi");
        assert(ob->get() == "ghi");
        assert(called);
    }
    
    //back() const
    {
        foo.vector.clear();
        foo.vector.emplace(foo.vector.begin(), "abc");
        foo.vector.emplace(std::next(foo.vector.begin()), "def");
        const auto& cobs = obs.get<vector>();
        assert(cobs.back() == "def");
    }
    
    //begin()
    {
        foo.vector.clear();
        foo.vector.emplace(foo.vector.begin(), "abc");
        auto ob = *obs.get<vector>().begin();
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
        foo.vector.clear();
        foo.vector.emplace(foo.vector.begin(), "abc");
        auto ob = *obs.get<vector>().cbegin();
        assert(ob == "abc");
    }
    
    //end/cend()
    {
        foo.vector.clear();
        foo.vector.emplace(foo.vector.begin(), "abc");
        assert(obs.get<vector>().begin() != obs.get<vector>().end());
        assert(obs.get<vector>().cbegin() != obs.get<vector>().cend());
    }

    //empty()
    {
        foo.vector.clear();
        assert(obs.get<vector>().empty());
    }
    
    //size()
    {
        foo.vector.clear();
        auto it = foo.vector.begin();
        foo.vector.emplace(it, "abc");
        foo.vector.emplace(++it, "def");
        assert(obs.get<vector>().size() == 2);
    }
    
    //max_size()
    {
        foo.vector.clear();
        auto it = foo.vector.begin();
        foo.vector.emplace(it, "abc");
        foo.vector.emplace(++it, "def");
        assert(obs.get<vector>().max_size() >= 2);
    }
    
     //clear()
    {
        foo.vector.clear();
        auto it = foo.vector.begin();
        foo.vector.emplace(it, "abc");
        foo.vector.emplace(++it, "def");
        bool called{false};
        obs.get<vector>().on_erase([&called](const vector_t&, vector_t::const_iterator)
                                   { called = true; });
        obs.get<vector>().clear();
        assert(called);
        assert(obs.get<vector>().empty());
    }
        
     //reserve()
    {
        foo.vector.clear();
        obs.get<vector>().reserve(100);
    }
    
    //capacity()
    {
        foo.vector.clear();
        auto it = foo.vector.begin();
        foo.vector.emplace(it, "abc");
        assert(obs.get<vector>().capacity() >= 1);
    }

    //shrink_to_fit()
    {
        foo.vector.clear();
        auto it = foo.vector.begin();
        foo.vector.emplace(it, "abc");
        obs.get<vector>().shrink_to_fit();
    }
     //erase()
    {
        foo.vector.clear();
        auto it = foo.vector.begin();
        foo.vector.emplace(it, "abc");
        foo.vector.emplace(++it, "def");
        bool called{false};
        obs.get<vector>().on_erase([&called](const vector_t&, vector_t::const_iterator)
                                   { called = true; });
        obs.get<vector>().erase(obs.get<vector>().begin());
        assert(called);
        assert(obs.get<vector>().size() == 1);
    }
    
     //erase(first, last)
    {
        foo.vector.clear();
        auto it = foo.vector.begin();
        foo.vector.emplace(it, "abc");
        foo.vector.emplace(++it, "def");
        bool called{false};
        obs.get<vector>().on_erase([&called](const vector_t&, vector_t::const_iterator)
                                   { called = true; });
        obs.get<vector>().erase(obs.get<vector>().begin(),
                                obs.get<vector>().end());
        assert(called);
        assert(obs.get<vector>().empty());
    }
    
    //emplace()
    {
        foo.vector.clear();
        bool called{false};
        obs.get<vector>().on_insert([&called](const vector_t&, vector_t::const_iterator)
                                    { called = true; });
        auto it = foo.vector.begin();
        obs.get<vector>().emplace(it, "abc");
        obs.get<vector>().emplace(++it, "def");
        assert(called);
        assert(*obs.get<vector>().cbegin() == "abc");
        assert(*std::next(obs.get<vector>().cbegin()) == "def");
    }
    
    //push_back()
    {
        foo.vector.clear();
        bool called{false};
        obs.get<vector>().push_back("abc");
        boost::signals2::scoped_connection c =
            obs.get<vector>().on_insert([&called](const vector_t&,
                                                  vector_t::const_iterator it)
                                    {
                                        assert(*it == "def");
                                        called = true;
                                    });
        obs.get<vector>().push_back("def");
        assert(called);
        assert(*obs.get<vector>().cbegin() == "abc");
        assert(*std::next(obs.get<vector>().cbegin()) == "def");
    }
    
    //emplace_back()
    {
        foo.vector.clear();
        bool called{false};
        obs.get<vector>().push_back("abc");
        boost::signals2::scoped_connection c =
            obs.get<vector>().on_insert([&called](const vector_t&,
                                                  vector_t::const_iterator it)
                                    {
                                        assert(*it == "def");
                                        called = true;
                                    });
        obs.get<vector>().emplace_back("def");
        assert(called);
        assert(*obs.get<vector>().cbegin() == "abc");
        assert(*std::next(obs.get<vector>().cbegin()) == "def");
    }
    
    //pop_back()
    {
        foo.vector.clear();
        bool called{false};
        obs.get<vector>().push_back("abc");
        boost::signals2::scoped_connection c =
            obs.get<vector>().on_erase([&called](const vector_t&,
                                                  vector_t::const_iterator it)
                                    {
                                        called = true;
                                    });
        obs.get<vector>().pop_back();
        assert(called);
        assert(obs.get<vector>().empty());
    }
    
    //insert(pos, const value_type&)
    {
        foo.vector.clear();
        bool called{false};
        obs.get<vector>().on_insert([&called](const vector_t&, vector_t::const_iterator)
                                    { called = true; });
        auto it = foo.vector.begin();
        std::string v = "abc";
        obs.get<vector>().insert(it, v);
        std::string v2 = "def";
        obs.get<vector>().insert(++it, v2);
        assert(called);
        assert(*obs.get<vector>().cbegin() == "abc");
        assert(*std::next(obs.get<vector>().cbegin()) == "def");
    }

    //insert(pos, value_type&&)
    {
        foo.vector.clear();
        bool called{false};
        obs.get<vector>().on_insert([&called](const vector_t&, vector_t::const_iterator)
                                    { called = true; });
        auto it = foo.vector.begin();
        obs.get<vector>().insert(it, "abc");
        obs.get<vector>().insert(++it, "def");
        assert(called);
        assert(*obs.get<vector>().cbegin() == "abc");
        assert(*std::next(obs.get<vector>().cbegin()) == "def");
    }
    
    //insert(pos, count, const value_type&)
    {
        foo.vector.clear();
        bool called{false};
        obs.get<vector>().on_insert([&called](const vector_t&, vector_t::const_iterator)
                                    { called = true; });
        auto it = foo.vector.begin();
        obs.get<vector>().insert(it, 2, "abc");
        assert(called);
        assert(*std::next(obs.get<vector>().cbegin()) == "abc");
    }
    
    //insert(pos, first, last)
    {
        foo.vector.clear();
        bool called{false};
        obs.get<vector>().on_insert([&called](const vector_t&, vector_t::const_iterator)
                                    { called = true; });
        std::array<std::string, 2> a{{"abc", "def"}};
        obs.get<vector>().insert(foo.vector.begin(), a.cbegin(), a.cend());
        assert(called);
        assert(*obs.get<vector>().cbegin() == "abc");
        assert(*std::next(obs.get<vector>().cbegin()) == "def");
    }

    //insert(ilist)
    {
        foo.vector.clear();
        bool called{false};
        obs.get<vector>().on_insert([&called](const vector_t&, vector_t::const_iterator)
                                    { called = true; });
        obs.get<vector>().insert(foo.vector.begin(), {"abc", "def"});
        assert(called);
        assert(*obs.get<vector>().cbegin() == "abc");
        assert(*std::next(obs.get<vector>().cbegin()) == "def");
    }

}
