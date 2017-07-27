#include "observable/class.hpp"
#include "observable/unordered_set.hpp"

#include <array>
#include <iostream>
#include <string>
#include <unordered_set>

using set_t = std::unordered_set<std::string>;

struct foo_t
{
    set_t set;
};

struct set{};

using obs_t = observable::class_<
    foo_t,
    observable::unordered_set<set_t, set>
    >;

int main()
{
    foo_t foo;
    obs_t obs(foo, foo.set);

    //emplace
    {
        bool ok{false};
        boost::signals2::scoped_connection c =
            obs.get<set>().on_insert(
                [&ok](const set_t&)
                {
                    ok = true;
                });
        obs.get<set>().emplace("abc");
        assert(ok);
    }

    //emplace_hint fail
    {
        bool ok{false};
        boost::signals2::scoped_connection c =
            obs.get<set>().on_insert(
                [&ok](const set_t&)
                {
                    ok = false;
                });
        obs.get<set>().emplace_hint(foo.set.begin(), "abc");
        assert(!ok);
    }
    
    //emplace_hint success
    {
        bool ok{false};
        boost::signals2::scoped_connection c =
            obs.get<set>().on_insert(
                [&ok](const set_t&)
                {
                    ok = true;
                });
        obs.get<set>().emplace_hint(foo.set.begin(), "def");
        assert(ok);
    }
    
    //size
    {
        assert(obs.get<set>().size() == 2);
    }
    
    //begin/cbegin()
    {
        assert(*obs.get<set>().begin() == "def");
        assert(*obs.get<set>().cbegin() == "def");
    }
    
    //end/cend()
    {
        assert(obs.get<set>().end() != obs.get<set>().begin());
        assert(obs.get<set>().cend() != obs.get<set>().cbegin());
    }
    
    //insert lvalue success
    {
        bool ok{false};
        boost::signals2::scoped_connection c =
            obs.get<set>().on_insert(
                [&ok](const set_t&)
                {
                    ok = true;
                });
        auto e = "ghi";
        obs.get<set>().insert(e);
        assert(ok);
    }
        
    //insert rvalue success
    {
        bool ok{false};
        boost::signals2::scoped_connection c =
            obs.get<set>().on_insert(
                [&ok](const set_t&)
                {
                    ok = true;
                });
        obs.get<set>().insert("jkl");
        assert(ok);
    }
    
    //insert_hint lvalue success
    {
        bool ok{false};
        boost::signals2::scoped_connection c =
            obs.get<set>().on_insert(
                [&ok](const set_t&)
                {
                    ok = true;
                });
        auto e = "mno";
        obs.get<set>().insert_hint(obs.get<set>().begin(), e);
        assert(ok);
    }
    
    //insert_hint rvalue success
    {
        bool ok{false};
        boost::signals2::scoped_connection c =
            obs.get<set>().on_insert(
                [&ok](const set_t&)
                {
                    ok = true;
                });
        obs.get<set>().insert_hint(obs.get<set>().begin(), "pqr");
        assert(ok);
    }
    
    //insert(first, last) success
    {
        bool ok{false};
        boost::signals2::scoped_connection c =
            obs.get<set>().on_insert(
                [&ok](const set_t&)
                {
                    ok = true;
                });
        std::array<std::string, 2> a{{"123", "456"}};
        obs.get<set>().insert(a.begin(), a.end());
        assert(ok);
    }
        
    //insert(ilist) success
    {
        bool ok{false};
        boost::signals2::scoped_connection c =
            obs.get<set>().on_insert(
                [&ok](const set_t&)
                {
                    ok = true;
                });
        obs.get<set>().insert({"789", "012"});
        assert(ok);
    }
                        
    //count
    {
        assert(obs.get<set>().count("abc") == 1);
        assert(obs.get<set>().count("<invalid>") == 0);
    }
    
    //find
    {
        assert(obs.get<set>().find("abc") != obs.get<set>().end());
        assert(obs.get<set>().find("<invalid>") == obs.get<set>().end());
    }
    
    //equal_range
    {
        assert(obs.get<set>().equal_range("abc").first != obs.get<set>().end());
        assert(obs.get<set>().equal_range("<invalid>").first == obs.get<set>().end());
    }
    
    //swap
    {
        bool ok{false};
        boost::signals2::scoped_connection c =
            obs.get<set>().on_insert(
                [&ok](const set_t&)
                {
                    ok = true;
                });
        set_t other{"098", "765"};
        obs.get<set>().swap(other);
        assert(ok);
    }
    
    //erase
    {
        bool ok{false};
        boost::signals2::scoped_connection c =
            obs.get<set>().on_erase(
                [&ok](const set_t&)
                {
                    ok = true;
                });
        obs.get<set>().erase(foo.set.begin());
        assert(ok);
    }
    
    //erase(first, last)
    {
        bool ok{false};
        obs.get<set>().emplace("ghi");
        boost::signals2::scoped_connection c =
            obs.get<set>().on_erase(
                [&ok](const set_t&)
                {
                    ok = true;
                });
        obs.get<set>().erase(foo.set.begin(), foo.set.end());
        assert(ok);
    }
    
    //erase(key) success
    {
        bool ok{false};
        obs.get<set>().emplace("abc");
        obs.get<set>().emplace("def");
        obs.get<set>().emplace("ghi");
        boost::signals2::scoped_connection c =
            obs.get<set>().on_erase(
                [&ok](const set_t&)
                {
                    ok = true;
                });
        obs.get<set>().erase("abc");
        assert(ok);
    }
    
    //erase(key) success
    {
        bool ok{true};
        obs.get<set>().emplace("abc");
        obs.get<set>().emplace("def");
        obs.get<set>().emplace("ghi");
        boost::signals2::scoped_connection c =
            obs.get<set>().on_erase(
                [&ok](const set_t&)
                {
                    ok = false;
                });
        obs.get<set>().erase("<invalid>");
        assert(ok);
    }
    
    //clear
    {
        bool ok{false};
        boost::signals2::scoped_connection c =
            obs.get<set>().on_erase(
                [&ok](const set_t&)
                {
                    ok = true;
                });
        obs.get<set>().clear();
        assert(ok);
    }
    
    //empty
    {
        assert(obs.get<set>().empty());
    }
}

