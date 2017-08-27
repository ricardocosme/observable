#include "observable/vector.hpp"

#include <array>
#include <string>
#include <vector>

using vector_t = std::vector<std::string>;

using obs_t = observable::vector<vector_t>;

int main()
{
    vector_t vec;;
    obs_t obs(vec);

    //vector_iterator default constructed
    {
        obs_t::iterator i;
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
        vec.clear();
        vec.emplace(vec.begin(), "abc");
        try
        {
            bool called{false};
            auto ob = obs.at(0);
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
        vec.clear();
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
        vec.clear();
        vec.emplace(vec.begin(), "abc");
        try
        {
            const auto& co = obs;
            auto v = co.at(0);
            assert(v == "abc");
        }
        catch(const std::out_of_range&)
        { ok = false; }
        assert(ok);            
    }

    //operator[] 
    {
        vec.clear();
        vec.emplace(vec.begin(), "abc");
        bool called{false};
        auto ob = obs[0];
        assert(ob->get() == "abc");
        ob->on_change([&called](const std::string&)
                     { called = true; });
        ob->assign("def");
        assert(ob->get() == "def");
        assert(called);
    }

    //operator[] const
    {
        vec.clear();
        vec.emplace(vec.begin(), "abc");
        const auto& cobs = obs;
        assert(cobs[0] == "abc");
    }
    
    //front()
    {
        vec.clear();
        vec.emplace(vec.begin(), "abc");
        auto ob = obs.front();
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
        vec.clear();
        vec.emplace(vec.begin(), "abc");
        auto& cobs = obs;
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
        vec.clear();
        vec.emplace(vec.begin(), "abc");
        vec.emplace(std::next(vec.begin()), "def");
        auto ob = obs.back();
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
        vec.clear();
        vec.emplace(vec.begin(), "abc");
        vec.emplace(std::next(vec.begin()), "def");
        const auto& cobs = obs;
        assert(cobs.back() == "def");
    }
    
    //begin()
    {
        vec.clear();
        vec.emplace(vec.begin(), "abc");
        auto ob = *obs.begin();
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
        vec.clear();
        vec.emplace(vec.begin(), "abc");
        auto ob = *obs.cbegin();
        assert(ob == "abc");
    }
    
    //end/cend()
    {
        vec.clear();
        vec.emplace(vec.begin(), "abc");
        assert(obs.begin() != obs.end());
        assert(obs.cbegin() != obs.cend());
    }

    //empty()
    {
        vec.clear();
        assert(obs.empty());
    }
    
    //size()
    {
        vec.clear();
        auto it = vec.begin();
        vec.emplace(it, "abc");
        vec.emplace(++it, "def");
        assert(obs.size() == 2);
    }
    
    //max_size()
    {
        vec.clear();
        auto it = vec.begin();
        vec.emplace(it, "abc");
        vec.emplace(++it, "def");
        assert(obs.max_size() >= 2);
    }
    
     //clear()
    {
        vec.clear();
        auto it = vec.begin();
        vec.emplace(it, "abc");
        vec.emplace(++it, "def");
        bool called{false};
        obs.on_erase([&called](const vector_t&, vector_t::const_iterator)
                                   { called = true; });
        obs.clear();
        assert(called);
        assert(obs.empty());
    }
        
     //reserve()
    {
        vec.clear();
        obs.reserve(100);
    }
    
    //capacity()
    {
        vec.clear();
        auto it = vec.begin();
        vec.emplace(it, "abc");
        assert(obs.capacity() >= 1);
    }

    //shrink_to_fit()
    {
        vec.clear();
        auto it = vec.begin();
        vec.emplace(it, "abc");
        obs.shrink_to_fit();
    }
     //erase()
    {
        vec.clear();
        auto it = vec.begin();
        vec.emplace(it, "abc");
        vec.emplace(++it, "def");
        bool called{false};
        obs.on_erase([&called](const vector_t&, vector_t::const_iterator)
                                   { called = true; });
        obs.erase(obs.begin());
        assert(called);
        assert(obs.size() == 1);
    }
    
     //erase(first, last)
    {
        vec.clear();
        auto it = vec.begin();
        vec.emplace(it, "abc");
        vec.emplace(++it, "def");
        bool called{false};
        obs.on_erase([&called](const vector_t&, vector_t::const_iterator)
                                   { called = true; });
        obs.erase(obs.begin(),
                                obs.end());
        assert(called);
        assert(obs.empty());
    }
    
    //emplace()
    {
        vec.clear();
        bool called{false};
        obs.on_insert([&called](const vector_t&, vector_t::const_iterator)
                                    { called = true; });
        auto it = vec.begin();
        obs.emplace(it, "abc");
        obs.emplace(++it, "def");
        assert(called);
        assert(*obs.cbegin() == "abc");
        assert(*std::next(obs.cbegin()) == "def");
    }
    
    //push_back()
    {
        vec.clear();
        bool called{false};
        obs.push_back("abc");
        boost::signals2::scoped_connection c =
            obs.on_insert([&called](const vector_t&,
                                                  vector_t::const_iterator it)
                                    {
                                        assert(*it == "def");
                                        called = true;
                                    });
        obs.push_back("def");
        assert(called);
        assert(*obs.cbegin() == "abc");
        assert(*std::next(obs.cbegin()) == "def");
    }
    
    //emplace_back()
    {
        vec.clear();
        bool called{false};
        obs.push_back("abc");
        boost::signals2::scoped_connection c =
            obs.on_insert([&called](const vector_t&,
                                                  vector_t::const_iterator it)
                                    {
                                        assert(*it == "def");
                                        called = true;
                                    });
        obs.emplace_back("def");
        assert(called);
        assert(*obs.cbegin() == "abc");
        assert(*std::next(obs.cbegin()) == "def");
    }
    
    //pop_back()
    {
        vec.clear();
        bool called{false};
        obs.push_back("abc");
        boost::signals2::scoped_connection c =
            obs.on_erase([&called](const vector_t&,
                                                  vector_t::const_iterator it)
                                    {
                                        called = true;
                                    });
        obs.pop_back();
        assert(called);
        assert(obs.empty());
    }
    
    //insert(pos, const value_type&)
    {
        vec.clear();
        bool called{false};
        obs.on_insert([&called](const vector_t&, vector_t::const_iterator)
                                    { called = true; });
        auto it = vec.begin();
        std::string v = "abc";
        obs.insert(it, v);
        std::string v2 = "def";
        obs.insert(++it, v2);
        assert(called);
        assert(*obs.cbegin() == "abc");
        assert(*std::next(obs.cbegin()) == "def");
    }

    //insert(pos, value_type&&)
    {
        vec.clear();
        bool called{false};
        obs.on_insert([&called](const vector_t&, vector_t::const_iterator)
                                    { called = true; });
        auto it = vec.begin();
        obs.insert(it, "abc");
        obs.insert(++it, "def");
        assert(called);
        assert(*obs.cbegin() == "abc");
        assert(*std::next(obs.cbegin()) == "def");
    }
    
    //insert(pos, count, const value_type&)
    {
        vec.clear();
        bool called{false};
        obs.on_insert([&called](const vector_t&, vector_t::const_iterator)
                                    { called = true; });
        auto it = vec.begin();
        obs.insert(it, 2, "abc");
        assert(called);
        assert(*std::next(obs.cbegin()) == "abc");
    }
    
    //insert(pos, first, last)
    {
        vec.clear();
        bool called{false};
        obs.on_insert([&called](const vector_t&, vector_t::const_iterator)
                                    { called = true; });
        std::array<std::string, 2> a{{"abc", "def"}};
        obs.insert(vec.begin(), a.cbegin(), a.cend());
        assert(called);
        assert(*obs.cbegin() == "abc");
        assert(*std::next(obs.cbegin()) == "def");
    }

    //insert(ilist)
    {
        vec.clear();
        bool called{false};
        obs.on_insert([&called](const vector_t&, vector_t::const_iterator)
                                    { called = true; });
        obs.insert(vec.begin(), {"abc", "def"});
        assert(called);
        assert(*obs.cbegin() == "abc");
        assert(*std::next(obs.cbegin()) == "def");
    }

}
