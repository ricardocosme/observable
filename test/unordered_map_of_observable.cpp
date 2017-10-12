#include <observable/unordered_map.hpp>
#include <observable/value.hpp>

#include <string>

using namespace observable;

using umap_t = unordered_map<int, value<std::string>>;

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
        umap_t umap;
    }
    
    {
        umap_t umap(10);
    }
    
    //begin()
    {
        umap_t umap;
        umap.emplace(2, std::string{"abc"});
        auto& e = *umap.begin();
        assert(e.first == 2);
        assert(e.second == std::string{"abc"});
        bool on_value_change_called{false};
        umap.on_value_change
            ([&on_value_change_called](const umap_t&, const umap_t::value_type&)
             {
                 on_value_change_called = true;
             });
        bool on_change_called{false};
        umap.on_change
            ([&on_change_called](const umap_t&)
             {
                 on_change_called = true;
             });
        e.second = "change";
        assert(on_value_change_called);
        assert(on_change_called);
    }

    //cbegin()
    {
        umap_t umap;
        umap.emplace(2, std::string{"abc"});
        auto& e = *umap.cbegin();
        assert(e.first == 2);
        assert(e.second == std::string{"abc"});
    }
    
    //end/cend()
    {
        umap_t umap;
        umap.emplace(2, std::string{"abc"});
        umap.emplace(3, std::string{"def"});
        assert(std::distance(umap.begin(), umap.end()) == 2);
        assert(std::distance(umap.cbegin(), umap.cend()) == 2);
    }

    //empty()
    {
        umap_t umap;
        assert(umap.empty());
        umap.emplace(2, std::string{"abc"});
        assert(!umap.empty());
    }
    
    //size()
    {
        umap_t umap;
        umap.emplace(2, std::string{"abc"});
        umap.emplace(3, std::string{"def"});
        assert(umap.size() == 2);
    }
    
    //max_size()
    {
        umap_t umap;
        umap.emplace(2, std::string{"abc"});
        umap.emplace(3, std::string{"def"});
        assert(umap.max_size() >= 2);
    }
    
    //clear() before_erase returns true
    {
        umap_t umap;
        umap.emplace(0, std::string{"abc"});
        umap.emplace(1, std::string{"def"});
        bool after_called{false};
        bool before_called{false};
        bool on_change_called{false};
        umap.before_erase([&before_called]
                         (const umap_t& umap,
                          umap_t::const_iterator it)
                         {
                             before_called = true;
                             assert(umap.cend() == it);
                             return true;
                         });
        umap.after_erase([&after_called]
                        (const umap_t&,
                         umap_t::const_iterator)
                        { after_called = true; });
        umap.on_change([&on_change_called]
                      (const umap_t&)
                      { on_change_called = true; });
        assert(umap.clear());
        assert(before_called);
        assert(after_called);
        assert(on_change_called);
    }
            
    //clear() before_erase returns false
    {
        umap_t umap;
        umap.emplace(0, std::string{"abc"});
        umap.emplace(2, std::string{"def"});
        bool after_called{false};
        bool before_called{false};
        bool on_change_called{false};
        umap.before_erase([&before_called]
                         (const umap_t& umap,
                          umap_t::const_iterator it)
                         {
                             before_called = true;
                             assert(umap.cend() == it);
                             return false;
                         });
        umap.after_erase([&after_called]
                        (const umap_t&,
                         umap_t::const_iterator)
                        { after_called = true; });
        umap.on_change([&on_change_called]
                      (const umap_t&)
                      { on_change_called = true; });
        assert(!umap.clear());
        assert(before_called);
        assert(!after_called);
        assert(!on_change_called);
    }
    
    //erase(const_iterator) before_erase returns true
    {
        umap_t umap;
        umap.emplace(0, std::string{"abc"});
        umap.emplace(1, std::string{"def"});
        bool after_called{false};
        bool before_called{false};
        bool on_change_called{false};
        umap.before_erase([&before_called]
                         (const umap_t& umap,
                          umap_t::const_iterator it)
                         {
                             before_called = true;
                             assert(it->first == 0);
                             assert(it->second == std::string{"abc"});
                             return true;
                         });
        umap.after_erase([&after_called]
                        (const umap_t&,
                         umap_t::const_iterator)
                        { after_called = true; });
        umap.on_change([&on_change_called]
                      (const umap_t&)
                      { on_change_called = true; });
        auto ret = umap.erase(umap.find(0));
        assert(ret.second);
        assert(umap.cbegin()->second == std::string{"def"});
        assert(umap.size() == 1);
        assert(before_called);
        assert(after_called);
        assert(on_change_called);
    }
    
    //erase(const_iterator) before_erase returns false
    {
        umap_t umap;
        umap.emplace(0, std::string{"abc"});
        umap.emplace(1, std::string{"def"});
        bool after_called{false};
        bool before_called{false};
        bool on_change_called{false};
        umap.before_erase([&before_called]
                         (const umap_t& umap,
                          umap_t::const_iterator it)
                         {
                             before_called = true;
                             assert(it->first == 0);
                             assert(it->second == std::string{"abc"});
                             return false;
                         });
        umap.after_erase([&after_called]
                        (const umap_t&,
                         umap_t::const_iterator)
                        { after_called = true; });
        umap.on_change([&on_change_called]
                      (const umap_t&)
                      { on_change_called = true; });
        auto ret = umap.erase(umap.find(0));
        assert(ret.second == false);
        assert(ret.first == umap.end());
        assert(umap.size() == 2);
        assert(before_called);
        assert(!after_called);
        assert(!on_change_called);
    }
    
    //erase(first, last) before_erase returns true
    {
        umap_t umap;
        umap.emplace(0, std::string{"abc"});
        umap.emplace(1, std::string{"def"});
        bool called{false};
        bool before_called{false};
        bool on_change_called{false};
        umap.before_erase([&before_called]
                          (const umap_t& umap,
                           umap_t::const_iterator it)
                         {
                             before_called = true;
                             assert(umap.cbegin() == it);
                             return true;
                         });
        umap.after_erase([&called](const umap_t&,
                                  umap_t::const_iterator)
                        { called = true; });
        umap.on_change([&on_change_called]
                      (const umap_t&)
                      { on_change_called = true; });
        auto ret = umap.erase(umap.cbegin(), umap.cend());
        assert(ret.second);
        assert(ret.first == umap.end());
        assert(before_called);
        assert(called);
        assert(on_change_called);
        assert(umap.empty());
    }
    
    //erase(first, last) before_erase returns false
    {
        umap_t umap;
        umap.emplace(0, std::string{"abc"});
        umap.emplace(1, std::string{"def"});
        bool called{false};
        bool before_called{false};
        bool on_change_called{false};
        umap.before_erase([&before_called]
                          (const umap_t& umap,
                           umap_t::const_iterator it)
                         {
                             before_called = true;
                             assert(umap.cbegin() == it);
                             return false;
                         });
        umap.after_erase([&called](const umap_t&,
                                  umap_t::const_iterator)
                        { called = true; });
        umap.on_change([&on_change_called]
                      (const umap_t&)
                      { on_change_called = true; });
        auto ret = umap.erase(umap.cbegin(), umap.cend());
        assert(!ret.second);
        assert(ret.first == umap.end());
        assert(before_called);
        assert(!called);
        assert(!on_change_called);
        assert(!umap.empty());
    }
    
    //emplace()
    {
        umap_t umap;
        bool called{false};
        bool on_change_called{false};
        umap.emplace(0, std::string{"abc"});
        umap.after_insert([&called](const umap_t&,
                                   umap_t::const_iterator it)
                       {
                           assert(it->first == 1);
                           assert(it->second == std::string{"def"});
                           called = true;
                       });
        umap.on_change([&on_change_called]
                      (const umap_t&)
                      { on_change_called = true; });
        umap.emplace(1, std::string{"def"});
        assert(called);
        assert(on_change_called);
        assert(umap.size() == 2);
    }
    
    //emplace_hint()
    {
        umap_t umap;
        bool called{false};
        bool on_change_called{false};
        umap.emplace(0, std::string{"abc"});
        umap.after_insert([&called](const umap_t&,
                                   umap_t::const_iterator it)
                       {
                           assert(it->first == 1);
                           assert(it->second == std::string{"def"});
                           called = true;
                       });
        umap.on_change([&on_change_called]
                      (const umap_t&)
                      { on_change_called = true; });
        umap.emplace_hint(umap.cbegin(), 1, std::string{"def"});
        assert(called);
        assert(on_change_called);
        assert(umap.size() == 2);
    }

    //insert(value_type&&)
    {
        umap_t umap;
        bool called{false};
        bool on_change_called{false};
        umap_t::value_type v(0, std::string{"abc"});
        umap.insert(std::move(v));
        umap.after_insert([&called](const umap_t&,
                                   umap_t::const_iterator it)
                         {
                             assert(it->first == 1);
                             assert(it->second == std::string{"def"});
                             called = true;
                         });
        umap.on_change([&on_change_called]
                      (const umap_t&)
                      { on_change_called = true; });
        umap_t::value_type v2(1, std::string{"def"});
        auto ret = umap.insert(std::move(v2));
        assert(ret.second);
        assert(ret.first->first == 1);
        assert(ret.first->second == std::string{"def"});
        assert(called);
        assert(on_change_called);
        assert(umap.size() == 2);
    }

    //insert(const_iterator, value_type&&)
    {
        umap_t umap;
        bool called{false};
        bool on_change_called{false};
        umap_t::value_type v(0, std::string{"abc"});
        umap.insert(umap.cbegin(), std::move(v));
        umap.after_insert([&called](const umap_t&,
                                   umap_t::const_iterator it)
                         {
                             assert(it->first == 1);
                             assert(it->second == std::string{"def"});
                             called = true;
                         });
        umap.on_change([&on_change_called]
                      (const umap_t&)
                      { on_change_called = true; });
        umap_t::value_type v2(1, std::string{"def"});
        auto it = umap.insert(std::next(umap.cbegin()), std::move(v2));
        assert(it->first == 1);
        assert(it->second == std::string{"def"});
        assert(called);
        assert(on_change_called);
        assert(umap.size() == 2);
    }

    //insert(first, last)
    {
        umap_t umap;
        bool called{false};
        bool on_change_called{false};
        umap.after_insert([&called](const umap_t& umap,
                                   umap_t::const_iterator it)
                         {
                             assert(it == umap.cend());
                             called = true;
                         });
        umap.on_change([&on_change_called]
                      (const umap_t&)
                      { on_change_called = true; });
        std::vector<umap_t::value_type> src;
        src.emplace_back(0, std::string{"abc"});
        src.emplace_back(1, std::string{"def"});
        umap.insert(src.begin(), src.end());
        assert(called);
        assert(on_change_called);
        assert(umap.size() == 2);
    }

    //at fail
    {
        umap_t umap;
        bool ok{false};
        try
        {
            umap.at(0);
        }
        catch(const std::out_of_range&)
        { ok = true; }
        assert(ok);
    }

    //at success
    {
        bool ok{true};
        umap_t umap;
        umap.emplace(0, std::string{"abc"});
        try
        {
            assert(umap.at(0) == std::string{"abc"});
            bool on_value_change_called{false};
            bool on_change_called{false};
            umap.on_value_change
                ([&on_value_change_called]
                 (const umap_t&,
                  const umap_t::value_type& e)
                 {
                     assert(e.second == std::string{"change"});
                     on_value_change_called = true;
                 });
            umap.on_change
                ([&on_change_called](const umap_t&)
                 {
                     on_change_called = true;
                 });
            umap[0] = "change";
            assert(on_value_change_called);
            assert(on_change_called);
        }
        catch(const std::out_of_range&)
        { ok = false; }
        assert(ok);
    }

    //at const fail
    {
        umap_t umap;
        bool ok{false};
        try
        {
            const auto& cvalue = umap;
            cvalue.at(0);
        }
        catch(const std::out_of_range&)
        { ok = true; }
        assert(ok);
    }

    //at const success
    {
        bool ok{true};
        umap_t umap;
        umap.emplace(0, std::string{"abc"});
        try
        {
            const auto& cumap = umap;
            auto& cvalue = cumap.at(0);
            assert(cvalue == std::string{"abc"});
        }
        catch(const std::out_of_range&)
        { ok = false; }
        assert(ok);            
    }
    
    //operator[lvalue] 
    {
        umap_t umap;
        umap.emplace(0, std::string{"abc"});
        assert(umap[0] == std::string{"abc"});
        bool on_value_change_called{false};
        bool on_change_called{false};
        umap.on_value_change
            ([&on_value_change_called](const umap_t&,
                                       const umap_t::value_type& e)
             {
                 assert(e.second == std::string{"change"});
                 on_value_change_called = true;
             });
        umap.on_change
            ([&on_change_called](const umap_t&)
             {
                 on_change_called = true;
             });
        int i = 0;
        umap[i] = "change";
        assert(on_value_change_called);
        assert(on_change_called);
    }

    //operator[rvalue] 
    {
        umap_t umap;
        umap.emplace(0, std::string{"abc"});
        assert(umap[0] == std::string{"abc"});
        bool on_value_change_called{false};
        bool on_change_called{false};
        umap.on_value_change
            ([&on_value_change_called](const umap_t&,
                                       const umap_t::value_type& e)
             {
                 assert(e.second == std::string{"change"});
                 on_value_change_called = true;
             });
        umap.on_change
            ([&on_change_called](const umap_t&)
             {
                 on_change_called = true;
             });
        umap[0] = "change";
        assert(on_value_change_called);
        assert(on_change_called);
    }
    
    // //swap
    // {
    //     umap.clear();
    //     bool ok{false};
    //     boost::signals2::scoped_connection c =
    //         umap().on_insert(
    //             [&ok](umap_t::const_iterator)
    //             {
    //                 ok = true;
    //             });
    //     umap_t other{{2, std::string{"abc"}}, {3, std::string{"def"}}};
    //     umap().swap(other);
    //     assert(ok);
    // }
    
    //count
    {
        umap_t umap;
        umap.emplace(2, std::string{"abc"});
        umap.emplace(1, std::string{"def"});
        assert(umap.count(2) == 1);
        assert(umap.count(0) == 0);
    }
    
    //find fail
    {
        umap_t umap;
        assert(umap.find(0) == umap.end());
        const auto& cumap = umap;
        assert(cumap.find(0) == umap.cend());
        
    }
    
    //find success
    {
        umap_t umap;
        umap.emplace(2, std::string{"abc"});
        umap.emplace(3, std::string{"def"});
        auto it = umap.find(2);
        assert(it != umap.end());
    }
    
    // // //equal_range
    // // {
    // //     umap.clear();
    // //     umap.emplace(2, std::string{"abc"});
    // //     umap.emplace(3, std::string{"def"});
    // //     assert(umap().equal_range(2).first != umap().end());
    // //     assert(umap().equal_range(4).first == umap().end());
    // // }

}
