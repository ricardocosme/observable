#include "observable/unordered_set.hpp"

#include <array>
#include <iostream>
#include <string>

using obs_t = observable::unordered_set<std::string>;

int main()
{
    obs_t obs;

    //emplace
    {
        bool ok{false};
        boost::signals2::scoped_connection c =
            obs.on_insert(
                [&ok](obs_t::const_iterator)
                {
                    ok = true;
                });
        obs.emplace("abc");
        assert(ok);
    }

    //emplace_hint fail
    {
        bool ok{false};
        boost::signals2::scoped_connection c =
            obs.on_insert(
                [&ok](obs_t::const_iterator)
                {
                    ok = false;
                });
        obs.emplace_hint(obs.begin(), "abc");
        assert(!ok);
    }
    
    //emplace_hint success
    {
        bool ok{false};
        boost::signals2::scoped_connection c =
            obs.on_insert(
                [&ok](obs_t::const_iterator)
                {
                    ok = true;
                });
        obs.emplace_hint(obs.begin(), "def");
        assert(ok);
    }
    
    //size
    {
        assert(obs.size() == 2);
    }
    
    //begin/cbegin()
    {
        assert(*obs.begin() == "def");
        assert(*obs.cbegin() == "def");
    }
    
    //end/cend()
    {
        assert(obs.end() != obs.begin());
        assert(obs.cend() != obs.cbegin());
    }
    
    //insert lvalue success
    {
        bool ok{false};
        boost::signals2::scoped_connection c =
            obs.on_insert(
                [&ok](obs_t::const_iterator)
                {
                    ok = true;
                });
        auto e = "ghi";
        obs.insert(e);
        assert(ok);
    }
        
    //insert rvalue success
    {
        bool ok{false};
        boost::signals2::scoped_connection c =
            obs.on_insert(
                [&ok](obs_t::const_iterator)
                {
                    ok = true;
                });
        obs.insert("jkl");
        assert(ok);
    }
    
    //insert_hint lvalue success
    {
        bool ok{false};
        boost::signals2::scoped_connection c =
            obs.on_insert(
                [&ok](obs_t::const_iterator)
                {
                    ok = true;
                });
        auto e = "mno";
        obs.insert_hint(obs.begin(), e);
        assert(ok);
    }
    
    //insert_hint rvalue success
    {
        bool ok{false};
        boost::signals2::scoped_connection c =
            obs.on_insert(
                [&ok](obs_t::const_iterator)
                {
                    ok = true;
                });
        obs.insert_hint(obs.begin(), "pqr");
        assert(ok);
    }
    
    //insert(first, last) success
    {
        bool ok{false};
        boost::signals2::scoped_connection c =
            obs.on_insert(
                [&ok](obs_t::const_iterator)
                {
                    ok = true;
                });
        std::array<std::string, 2> a{{"123", "456"}};
        obs.insert(a.begin(), a.end());
        assert(ok);
    }
        
    //insert(ilist) success
    {
        bool ok{false};
        boost::signals2::scoped_connection c =
            obs.on_insert(
                [&ok](obs_t::const_iterator)
                {
                    ok = true;
                });
        obs.insert({"789", "012"});
        assert(ok);
    }
                        
    //count
    {
        assert(obs.count("abc") == 1);
        assert(obs.count("<invalid>") == 0);
    }
    
    //find
    {
        assert(obs.find("abc") != obs.end());
        assert(obs.find("<invalid>") == obs.end());
    }
    
    //equal_range
    {
        assert(obs.equal_range("abc").first != obs.end());
        assert(obs.equal_range("<invalid>").first == obs.end());
    }
    
    // //swap
    // {
    //     bool ok{false};
    //     boost::signals2::scoped_connection c =
    //         obs.on_insert(
    //             [&ok](obs_t::const_iterator)
    //             {
    //                 ok = true;
    //             });
    //     obs_t other{"098", "765"};
    //     obs.swap(other);
    //     assert(ok);
    // }
    
    //erase
    {
        obs.clear();
        obs.emplace("abc");
        bool ok{false};
        boost::signals2::scoped_connection c =
            obs.on_erase(
                [&ok](obs_t::value_type v, obs_t::const_iterator)
                {
                    assert(v == "abc");
                    ok = true;
                });
        obs.erase(obs.begin());
        assert(ok);
    }
    
    //erase(first, last)
    {
        bool ok{false};
        obs.emplace("ghi");
        boost::signals2::scoped_connection c =
            obs.on_erase(
                [&ok](obs_t::value_type v, obs_t::const_iterator)
                {
                    ok = true;
                });
        obs.erase(obs.begin(), obs.end());
        assert(ok);
    }
    
    //erase(key) success
    {
        bool ok{false};
        obs.emplace("abc");
        obs.emplace("def");
        obs.emplace("ghi");
        boost::signals2::scoped_connection c =
            obs.on_erase(
                [&ok](obs_t::value_type v, obs_t::const_iterator)
                {
                    ok = true;
                });
        obs.erase("abc");
        assert(ok);
    }
    
    //erase(key) success
    {
        bool ok{true};
        obs.emplace("abc");
        obs.emplace("def");
        obs.emplace("ghi");
        boost::signals2::scoped_connection c =
            obs.on_erase(
                [&ok](obs_t::value_type v, obs_t::const_iterator)
                {
                    ok = false;
                });
        obs.erase("<invalid>");
        assert(ok);
    }
    
    //clear
    {
        bool ok{false};
        boost::signals2::scoped_connection c =
            obs.on_erase(
                [&ok](obs_t::value_type, obs_t::const_iterator)
                {
                    ok = true;
                });
        obs.clear();
        assert(ok);
    }
    
    //empty
    {
        assert(obs.empty());
    }
}

