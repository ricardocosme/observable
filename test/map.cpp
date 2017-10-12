#include "observable/map.hpp"

#include <string>

using namespace observable;

using map_t = map<int, std::string>;

template<typename ocontainer, typename expected_t>
void check_equal(const ocontainer& c, const expected_t& expected)
{
    assert(c.size() == expected.size());
    for(auto& e : c)
        assert(expected.at(e.first) == e.second);
}

int main()
{
    {
        map_t m;
    }
    
    {
        std::vector<std::pair<int, std::string>> src{{0, "abc"}, {1, "def"}};
        map_t m(src.begin(), src.end());
        assert(m.size() == 2);
        std::map<int, std::string> expected{{0, "abc"}, {1, "def"}};;
        check_equal(m, expected);
    }

    {
        map_t m({std::pair<const int, std::string>{0, "abc"},
                 std::pair<const int, std::string>{1, "def"}});
        assert(m.size() == 2);
        std::map<int, std::string> expected{{0, "abc"}, {1, "def"}};;
        check_equal(m, expected);
    }
    
    //begin()
    {
        map_t m;
        m.emplace(2, "abc");
        auto e = *m.begin();
        assert(e.first == 2);
        assert(e.second == "abc");
        bool on_value_change_called{false};
        m.on_value_change
            ([&on_value_change_called](const map_t&, const map_t::value_type&)
             {
                 on_value_change_called = true;
             });
        bool on_change_called{false};
        m.on_change
            ([&on_change_called](const map_t&)
             {
                 on_change_called = true;
             });
        e.second = "change";
        assert(on_value_change_called);
        assert(on_change_called);
    }

    //cbegin()
    {
        map_t m;
        m.emplace(2, "abc");
        auto e = *m.cbegin();
        assert(e.first == 2);
        assert(e.second == "abc");
    }
    
    //end/cend()
    {
        map_t m;
        m.emplace(2, "abc");
        m.emplace(3, "def");
        assert(std::distance(m.begin(), m.end()) == 2);
        assert(std::distance(m.cbegin(), m.cend()) == 2);
    }

    //empty()
    {
        map_t m;
        assert(m.empty());
        m.emplace(2, "abc");
        assert(!m.empty());
    }
    
    //size()
    {
        map_t m;
        m.emplace(2, "abc");
        m.emplace(3, "def");
        assert(m.size() == 2);
    }
    
    //max_size()
    {
        map_t m;
        m.emplace(2, "abc");
        m.emplace(3, "def");
        assert(m.max_size() >= 2);
    }
    
    //clear() before_erase returns true
    {
        map_t m;
        m.emplace(0, "abc");
        m.emplace(1, "def");
        bool after_called{false};
        bool before_called{false};
        bool on_change_called{false};
        m.before_erase([&before_called]
                         (const map_t& m,
                          map_t::const_iterator it)
                         {
                             before_called = true;
                             assert(m.cend() == it);
                             return true;
                         });
        m.after_erase([&after_called]
                        (const map_t&,
                         map_t::const_iterator)
                        { after_called = true; });
        m.on_change([&on_change_called]
                      (const map_t&)
                      { on_change_called = true; });
        assert(m.clear());
        assert(before_called);
        assert(after_called);
        assert(on_change_called);
    }
            
    //clear() before_erase returns false
    {
        map_t m;
        m.emplace(0, "abc");
        m.emplace(2, "def");
        bool after_called{false};
        bool before_called{false};
        bool on_change_called{false};
        m.before_erase([&before_called]
                         (const map_t& m,
                          map_t::const_iterator it)
                         {
                             before_called = true;
                             assert(m.cend() == it);
                             return false;
                         });
        m.after_erase([&after_called]
                        (const map_t&,
                         map_t::const_iterator)
                        { after_called = true; });
        m.on_change([&on_change_called]
                      (const map_t&)
                      { on_change_called = true; });
        assert(!m.clear());
        assert(before_called);
        assert(!after_called);
        assert(!on_change_called);
    }
    
    //erase(const_iterator) before_erase returns true
    {
        map_t m;
        m.emplace(0, "abc");
        m.emplace(1, "def");
        bool after_called{false};
        bool before_called{false};
        bool on_change_called{false};
        m.before_erase([&before_called]
                         (const map_t& m,
                          map_t::const_iterator it)
                         {
                             before_called = true;
                             assert(it->first == 0);
                             assert(it->second == "abc");
                             return true;
                         });
        m.after_erase([&after_called]
                        (const map_t&,
                         map_t::const_iterator)
                        { after_called = true; });
        m.on_change([&on_change_called]
                      (const map_t&)
                      { on_change_called = true; });
        auto ret = m.erase(m.find(0));
        assert(ret.second);
        assert(m.cbegin()->second == "def");
        assert(m.size() == 1);
        assert(before_called);
        assert(after_called);
        assert(on_change_called);
    }
    
    //erase(const_iterator) before_erase returns false
    {
        map_t m;
        m.emplace(0, "abc");
        m.emplace(1, "def");
        bool after_called{false};
        bool before_called{false};
        bool on_change_called{false};
        m.before_erase([&before_called]
                         (const map_t& m,
                          map_t::const_iterator it)
                         {
                             before_called = true;
                             assert(it->first == 0);
                             assert(it->second == "abc");
                             return false;
                         });
        m.after_erase([&after_called]
                        (const map_t&,
                         map_t::const_iterator)
                        { after_called = true; });
        m.on_change([&on_change_called]
                      (const map_t&)
                      { on_change_called = true; });
        auto ret = m.erase(m.find(0));
        assert(ret.second == false);
        assert(ret.first == m.end());
        assert(m.size() == 2);
        assert(before_called);
        assert(!after_called);
        assert(!on_change_called);
    }
    
    //erase(first, last) before_erase returns true
    {
        map_t m;
        m.emplace(0, "abc");
        m.emplace(1, "def");
        bool called{false};
        bool before_called{false};
        bool on_change_called{false};
        m.before_erase([&before_called]
                          (const map_t& m,
                           map_t::const_iterator it)
                         {
                             before_called = true;
                             assert(m.cbegin() == it);
                             return true;
                         });
        m.after_erase([&called](const map_t&,
                                  map_t::const_iterator)
                        { called = true; });
        m.on_change([&on_change_called]
                      (const map_t&)
                      { on_change_called = true; });
        auto ret = m.erase(m.cbegin(), m.cend());
        assert(ret.second);
        assert(ret.first == m.end());
        assert(before_called);
        assert(called);
        assert(on_change_called);
        assert(m.empty());
    }
    
    //erase(first, last) before_erase returns false
    {
        map_t m;
        m.emplace(0, "abc");
        m.emplace(1, "def");
        bool called{false};
        bool before_called{false};
        bool on_change_called{false};
        m.before_erase([&before_called]
                          (const map_t& m,
                           map_t::const_iterator it)
                         {
                             before_called = true;
                             assert(m.cbegin() == it);
                             return false;
                         });
        m.after_erase([&called](const map_t&,
                                  map_t::const_iterator)
                        { called = true; });
        m.on_change([&on_change_called]
                      (const map_t&)
                      { on_change_called = true; });
        auto ret = m.erase(m.cbegin(), m.cend());
        assert(!ret.second);
        assert(ret.first == m.end());
        assert(before_called);
        assert(!called);
        assert(!on_change_called);
        assert(!m.empty());
    }
    
    //emplace()
    {
        map_t m;
        bool called{false};
        bool on_change_called{false};
        m.emplace(0, "abc");
        m.after_insert([&called](const map_t&,
                                   map_t::const_iterator it)
                       {
                           assert(it->first == 1);
                           assert(it->second == "def");
                           called = true;
                       });
        m.on_change([&on_change_called]
                      (const map_t&)
                      { on_change_called = true; });
        m.emplace(1, "def");
        assert(called);
        assert(on_change_called);
        assert(m.size() == 2);
    }
    
    //emplace_hint()
    {
        map_t m;
        bool called{false};
        bool on_change_called{false};
        m.emplace(0, "abc");
        m.after_insert([&called](const map_t&,
                                   map_t::const_iterator it)
                       {
                           assert(it->first == 1);
                           assert(it->second == "def");
                           called = true;
                       });
        m.on_change([&on_change_called]
                      (const map_t&)
                      { on_change_called = true; });
        m.emplace_hint(m.cbegin(), 1, "def");
        assert(called);
        assert(on_change_called);
        assert(m.size() == 2);
    }

    //insert(const value_type&)
    {
        map_t m;
        map_t::value_type v(0, "abc");
        m.insert(v);
        bool called{false};
        bool on_change_called{false};
        m.after_insert([&called](const map_t&,
                                   map_t::const_iterator it)
                         {
                             assert(it->first == 1);
                             assert(it->second == "def");
                             called = true;
                         });
        m.on_change([&on_change_called]
                      (const map_t&)
                      { on_change_called = true; });
        map_t::value_type v2(1, "def");
        auto ret = m.insert(v2);
        assert(ret.second);
        assert(ret.first->first == 1);
        assert(ret.first->second == "def");
        assert(called);
        assert(on_change_called);
        assert(m.size() == 2);
    }

    //insert(value_type&&)
    {
        map_t m;
        bool called{false};
        bool on_change_called{false};
        map_t::value_type v(0, "abc");
        m.insert(std::move(v));
        m.after_insert([&called](const map_t&,
                                   map_t::const_iterator it)
                         {
                             assert(it->first == 1);
                             assert(it->second == "def");
                             called = true;
                         });
        m.on_change([&on_change_called]
                      (const map_t&)
                      { on_change_called = true; });
        map_t::value_type v2(1, "def");
        auto ret = m.insert(std::move(v2));
        assert(ret.second);
        assert(ret.first->first == 1);
        assert(ret.first->second == "def");
        assert(called);
        assert(on_change_called);
        assert(m.size() == 2);
    }

    //insert(const_iterator, const value_type&)
    {
        map_t m;
        map_t::value_type v(0, "abc");
        m.insert(m.cbegin(), v);
        bool called{false};
        bool on_change_called{false};
        m.after_insert([&called](const map_t&,
                                   map_t::const_iterator it)
                         {
                             assert(it->first == 1);
                             assert(it->second == "def");
                             called = true;
                         });
        m.on_change([&on_change_called]
                      (const map_t&)
                      { on_change_called = true; });
        map_t::value_type v2(1, "def");
        auto it = m.insert(std::next(m.cbegin()), v2);
        assert(it->first == 1);
        assert(it->second == "def");
        assert(called);
        assert(on_change_called);
        assert(m.size() == 2);
    }

    //insert(const_iterator, value_type&&)
    {
        map_t m;
        bool called{false};
        bool on_change_called{false};
        map_t::value_type v(0, "abc");
        m.insert(m.cbegin(), std::move(v));
        m.after_insert([&called](const map_t&,
                                   map_t::const_iterator it)
                         {
                             assert(it->first == 1);
                             assert(it->second == "def");
                             called = true;
                         });
        m.on_change([&on_change_called]
                      (const map_t&)
                      { on_change_called = true; });
        map_t::value_type v2(1, "def");
        auto it = m.insert(std::next(m.cbegin()), std::move(v2));
        assert(it->first == 1);
        assert(it->second == "def");
        assert(called);
        assert(on_change_called);
        assert(m.size() == 2);
    }

    //insert(first, last)
    {
        map_t m;
        bool called{false};
        bool on_change_called{false};
        m.after_insert([&called](const map_t& m,
                                   map_t::const_iterator it)
                         {
                             assert(it == m.cend());
                             called = true;
                         });
        m.on_change([&on_change_called]
                      (const map_t&)
                      { on_change_called = true; });
        std::initializer_list<map_t::value_type> src{{0, "abc"}, {1, "def"}};
        m.insert(src.begin(), src.end());
        assert(called);
        assert(on_change_called);
        assert(m.size() == 2);
    }

    //insert(ilist)
    {
        map_t m;
        bool called{false};
        bool on_change_called{false};
        m.after_insert([&called](const map_t& m,
                                   map_t::const_iterator it)
                         {
                             assert(it == m.cend());
                             called = true;
                         });
        m.on_change([&on_change_called]
                      (const map_t&)
                      { on_change_called = true; });
        m.insert({{0, "abc"}, {1, "def"}});
        assert(called);
        assert(on_change_called);
        assert(m.size() == 2);
    }
    
    //at fail
    {
        map_t m;
        bool ok{false};
        try
        {
            m.at(0);
        }
        catch(const std::out_of_range&)
        { ok = true; }
        assert(ok);
    }

    //at success
    {
        bool ok{true};
        map_t m;
        m.emplace(0, "abc");
        try
        {
            assert(m.at(0) == "abc");
            bool on_value_change_called{false};
            bool on_change_called{false};
            m.on_value_change
                ([&on_value_change_called]
                 (const map_t&,
                  const map_t::value_type& e)
                 {
                     assert(e.second == "change");
                     on_value_change_called = true;
                 });
            m.on_change
                ([&on_change_called](const map_t&)
                 {
                     on_change_called = true;
                 });
            m[0] = "change";
            assert(on_value_change_called);
            assert(on_change_called);
        }
        catch(const std::out_of_range&)
        { ok = false; }
        assert(ok);
    }

    //at const fail
    {
        map_t m;
        bool ok{false};
        try
        {
            const auto& cvalue = m;
            cvalue.at(0);
        }
        catch(const std::out_of_range&)
        { ok = true; }
        assert(ok);
    }

    //at const success
    {
        bool ok{true};
        map_t m;
        m.emplace(0, "abc");
        try
        {
            const auto& cmap = m;
            auto cvalue = cmap.at(0);
            assert(cvalue == "abc");
        }
        catch(const std::out_of_range&)
        { ok = false; }
        assert(ok);            
    }
    
    //operator[lvalue] 
    {
        map_t m;
        m.emplace(0, "abc");
        assert(m[0] == "abc");
        bool on_value_change_called{false};
        bool on_change_called{false};
        m.on_value_change
            ([&on_value_change_called](const map_t&,
                                       const map_t::value_type& e)
             {
                 assert(e.second == "change");
                 on_value_change_called = true;
             });
        m.on_change
            ([&on_change_called](const map_t&)
             {
                 on_change_called = true;
             });
        int i = 0;
        m[i] = "change";
        assert(on_value_change_called);
        assert(on_change_called);
    }

    //operator[rvalue] 
    {
        map_t m;
        m.emplace(0, "abc");
        assert(m[0] == "abc");
        bool on_value_change_called{false};
        bool on_change_called{false};
        m.on_value_change
            ([&on_value_change_called](const map_t&,
                                       const map_t::value_type& e)
             {
                 assert(e.second == "change");
                 on_value_change_called = true;
             });
        m.on_change
            ([&on_change_called](const map_t&)
             {
                 on_change_called = true;
             });
        m[0] = "change";
        assert(on_value_change_called);
        assert(on_change_called);
    }
    
    // //swap
    // {
    //     m.clear();
    //     bool ok{false};
    //     boost::signals2::scoped_connection c =
    //         map().on_insert(
    //             [&ok](map_t::const_iterator)
    //             {
    //                 ok = true;
    //             });
    //     map_t other{{2, "abc"}, {3, "def"}};
    //     map().swap(other);
    //     assert(ok);
    // }
    
    //count
    {
        map_t m;
        m.insert({ {2, "abc"}, {3, "def"} });
        assert(m.count(2) == 1);
        assert(m.count(0) == 0);
    }
    
    //find fail
    {
        map_t m;
        assert(m.find(0) == m.end());
        const auto& cmap = m;
        assert(cmap.find(0) == m.cend());
        
    }
    
    //find success
    {
        map_t m;
        m.emplace(2, "abc");
        m.emplace(3, "def");
        auto it = m.find(2);
        assert(it != m.end());
    }
    
    // //equal_range
    // {
    //     m.clear();
    //     m.emplace(2, "abc");
    //     m.emplace(3, "def");
    //     assert(map().equal_range(2).first != map().end());
    //     assert(map().equal_range(4).first == map().end());
    // }

}
