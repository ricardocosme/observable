#include <observable/unordered_map.hpp>

#include <string>
#include <iostream>
using namespace observable;

using unmap_t = unordered_map<int, std::string>;

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
        unmap_t umap;
    }
    
    {
        unmap_t umap(10);
    }
    
    {
        std::vector<std::pair<int, std::string>> src{{0, "abc"}, {1, "def"}};
        unmap_t umap(src.begin(), src.end());
        assert(umap.size() == 2);
        std::unordered_map<int, std::string> expected{{0, "abc"}, {1, "def"}};;
        check_equal(umap, expected);
    }

    {
        std::vector<std::pair<int, std::string>> src{{0, "abc"}, {1, "def"}};
        unmap_t umap({std::pair<const int, std::string>{0, "abc"},
                      std::pair<const int, std::string>{1, "def"}});
        assert(umap.size() == 2);
        std::unordered_map<int, std::string> expected{{0, "abc"}, {1, "def"}};;
        check_equal(umap, expected);
    }
    
    // //at fail
    // {
    //     bool ok{false};
    //     try
    //     {
    //         obs.at(0);
    //     }
    //     catch(const std::out_of_range&)
    //     { ok = true; }
    //     assert(ok);
    // }

    // //at success
    // {
    //     bool ok{true};
    //     obs.clear();
    //     obs.emplace(2, "abc");
    //     bool called{false};
    //     try
    //     {
    //         obs.at(2);
    //     //     assert(ob->get() == "abc");
    //     //     ob->on_change([&called](const std::string&)
    //     //                   { called = true; });
    //     //     ob->assign("def");
    //     //     assert(ob->get() == "def");
    //     //     assert(called);
    //     // }
    //     catch(const std::out_of_range&)
    //     { ok = false; }
    //     assert(ok);
    // }
    
    // //at const fail
    // {
    //     bool ok{false};
    //     try
    //     {
    //         const auto& co = obs;
    //         co.at(0);
    //     }
    //     catch(const std::out_of_range&)
    //     { ok = true; }
    //     assert(ok);
    // }

    // //at const success
    // {
    //     bool ok{true};
    //     obs.clear();
    //     obs.emplace(2, "abc");
    //     try
    //     {
    //         const auto& co = obs;
    //         auto v = co.at(2);
    //         assert(v == "abc");
    //     }
    //     catch(const std::out_of_range&)
    //     { ok = false; }
    //     assert(ok);            
    // }

    // //operator[] lvalue insert
    // {
    //     // bool called{false};
    //     obs_t::key_type k = 0;
    //     auto& ob = obs[k];
    //     assert(ob == obs_t::mapped_type{});
    //     // ob->on_change([&called]()
    //     //               { called = true; });
    //     // ob->assign("def");
    //     // assert(ob->get() == "def");
    //     // assert(called);
    // }

    // //operator[] lvalue lookup
    // {
    //     obs.clear();
    //     obs.emplace(2, "abc");
    //     obs_t::key_type k = 2;
    //     auto& ob = obs[k];
    //     assert(ob == "abc");
    //     // bool called{false};
    //     // ob->on_change([&called]()
    //     //              { called = true; });
    //     // ob->assign("def");
    //     // assert(ob->get() == "def");
    //     // assert(called);
    // }
    
    // //operator[] rvalue insert
    // {
    //     auto& ob = obs[1];
    //     assert(ob == obs_t::mapped_type{});
    // }

    // //operator[] rvalue lookup
    // {
    //     obs.clear();
    //     obs.emplace(2, "abc");
    //     auto& ob = obs[2];
    //     assert(ob == "abc");
    //     // bool called{false};
    //     // ob->on_change([&called]()
    //     //               { called = true; });
    //     // ob->assign("def");
    //     // assert(ob->get() == "def");
    //     // assert(called);
    // }

    // //begin()
    // {
    //     obs.clear();
    //     obs.emplace(2, "abc");
    //     auto& ob = obs.begin()->second;
    //     assert(ob == "abc");
    //     // bool called{false};
    //     // ob->on_change([&called]()
    //     //               { called = true; });
    //     // ob->assign("def");
    //     // assert(ob->get() == "def");
    //     // assert(called);
    // }

    // //cbegin()
    // {
    //     obs.clear();
    //     obs.emplace(2, "abc");
    //     auto& ob = obs.cbegin()->second;
    //     assert(ob == "abc");
    // }
    
    // //end/cend()
    // {
    //     obs.clear();
    //     obs.emplace(2, "abc");
    //     assert(obs.begin() != obs.end());
    //     assert(obs.cbegin() != obs.cend());
    // }

    // //empty()
    // {
    //     obs.clear();
    //     assert(obs.empty());
    // }
    
    // //size()
    // {
    //     obs.clear();
    //     obs.emplace(2, "abc");
    //     obs.emplace(3, "def");
    //     assert(obs.size() == 2);
    // }
    
    // //max_size()
    // {
    //     obs.clear();
    //     obs.emplace(2, "abc");
    //     obs.emplace(3, "def");
    //     assert(obs.max_size() >= 2);
    // }
    
    //  //clear()
    // {
    //     obs.clear();
    //     obs.emplace(2, "abc");
    //     obs.emplace(3, "def");
    //     bool called{false};
    //     bool before_called{false};
    //     boost::signals2::scoped_connection c1 =
    //         obs.before_erase
    //         ([&before_called](obs_t::const_iterator it)
    //          {
    //              before_called = true;
    //              assert(m.end() == it);
    //          });
    //     boost::signals2::scoped_connection c2 =
    //         obs.on_erase
    //         ([&called](obs_t::const_iterator)
    //          { called = true; });
    //     obs.clear();
    //     assert(before_called);
    //     assert(called);
    //     assert(obs.empty());
    // }
    
    // //erase()
    // {
    //     obs.clear();
    //     obs.emplace(2, "abc");
    //     obs.emplace(3, "def");
    //     bool called{false};
    //     bool before_called{false};
    //     boost::signals2::scoped_connection c1 =
    //         obs().before_erase
    //         ([&before_called](const obs_t& m, obs_t::const_iterator it)
    //          {
    //              before_called = true;
    //              assert(it != m.end());
    //          });
    //     boost::signals2::scoped_connection c2 =
    //         obs().on_erase
    //         ([&called](obs_t::const_iterator)
    //          { called = true; });
    //     obs().erase(obs().cbegin());
    //     assert(before_called);
    //     assert(called);
    //     assert(obs().size() == 1);
    // }
    
    //  //erase(first, last)
    // {
    //     obs.clear();
    //     obs.emplace(2, "abc");
    //     obs.emplace(3, "def");
    //     bool called{false};
    //     bool before_called{false};
    //     boost::signals2::scoped_connection c1 =
    //         obs().before_erase
    //         ([&before_called](obs_t::const_iterator it)
    //          {
    //              before_called = true;
    //              assert(it != m.end());
    //          });
    //     boost::signals2::scoped_connection c2 =
    //         obs().on_erase
    //         ([&called](obs_t::value_type, obs_t::const_iterator)
    //          { called = true; });
    //     obs().erase(obs().cbegin(), obs().cend());
    //     assert(before_called);
    //     assert(called);
    //     assert(obs().empty());
    // }
    
    //  //erase(key)
    // {
    //     obs.clear();
    //     obs.emplace(2, "abc");
    //     obs.emplace(3, "def");
    //     bool called{false};
    //     bool before_called{false};
    //     boost::signals2::scoped_connection c1 =
    //         obs().before_erase
    //         ([&before_called](obs_t::const_iterator it)
    //          {
    //              before_called = true;
    //              assert(it->second == "abc");
    //          });
    //     boost::signals2::scoped_connection c2 =
    //         obs().on_erase
    //         ([&called](const obs_t&,
    //                    obs_t::value_type v,
    //                    obs_t::const_iterator)
    //          {
    //              assert(v.second == "abc");
    //              called = true;
    //          });
    //     obs().erase(2);
    //     assert(called);
    //     auto crmap = obs().get();
    //     assert(crmap.find(3)->second == "def");
    // }
    
    // //emplace()
    // {
    //     obs.clear();
    //     bool called{false};
    //     obs().on_insert([&called](obs_t::const_iterator)
    //                              { called = true; });
    //     obs().emplace(2, "abc");
    //     obs().emplace(3, "def");
    //     assert(called);
    //     auto crmap = obs().get();
    //     assert(crmap.find(2)->second == "abc");
    //     assert(crmap.find(3)->second == "def");
    // }
    
    // //emplace_hint()
    // {
    //     obs.clear();
    //     bool called{false};
    //     obs().on_insert([&called](obs_t::const_iterator)
    //                              { called = true; });
    //     obs().emplace_hint(obs.cbegin(), 2, "abc");
    //     obs().emplace_hint(obs.cbegin(),3, "def");
    //     assert(called);
    //     auto crmap = obs().get();
    //     assert(crmap.find(2)->second == "abc");
    //     assert(crmap.find(3)->second == "def");
    // }
    
    // //insert(const value_type&)
    // {
    //     obs.clear();
    //     bool called{false};
    //     obs().on_insert([&called](obs_t::const_iterator)
    //                              { called = true; });
    //     auto v = obs_t::value_type(2, "abc");
    //     obs().insert(v);
    //     auto v2 = obs_t::value_type(3, "def");
    //     obs().insert(v2);
    //     assert(called);
    //     auto crmap = obs().get();
    //     assert(crmap.find(2)->second == "abc");
    //     assert(crmap.find(3)->second == "def");
    // }

    // //insert(value_type&&)
    // {
    //     obs.clear();
    //     bool called{false};
    //     obs().on_insert([&called](obs_t::const_iterator)
    //                              { called = true; });
    //     obs().insert(obs_t::value_type(2, "abc"));
    //     obs().insert(obs_t::value_type(3, "def"));
    //     assert(called);
    //     auto crmap = obs().get();
    //     assert(crmap.find(2)->second == "abc");
    //     assert(crmap.find(3)->second == "def");
    // }
    
    // //insert(hint, value_type&&)
    // {
    //     obs.clear();
    //     bool called{false};
    //     obs().on_insert([&called](obs_t::const_iterator)
    //                              { called = true; });
    //     obs().insert(obs.cbegin(), obs_t::value_type(2, "abc"));
    //     obs().insert(obs.cbegin(), obs_t::value_type(3, "def"));
    //     assert(called);
    //     auto crmap = obs().get();
    //     assert(crmap.find(2)->second == "abc");
    //     assert(crmap.find(3)->second == "def");
    // }
    
    // //insert(first, last)
    // {
    //     obs.clear();
    //     bool called{false};
    //     obs().on_insert([&called](obs_t::const_iterator)
    //                              { called = true; });
    //     std::array<std::pair<std::size_t, std::string>, 2> a{{ {2, "abc"}, {3, "def"} }};
    //     obs().insert(a.cbegin(), a.cend());
    //     assert(called);
    //     auto crmap = obs().get();
    //     assert(crmap.find(2)->second == "abc");
    //     assert(crmap.find(3)->second == "def");
    // }

    // //insert(ilist)
    // {
    //     obs.clear();
    //     bool called{false};
    //     obs().on_insert([&called](obs_t::const_iterator)
    //                              { called = true; });
    //     obs().insert({ {2, "abc"}, {3, "def"} });
    //     assert(called);
    //     auto crmap = obs().get();
    //     assert(crmap.find(2)->second == "abc");
    //     assert(crmap.find(3)->second == "def");
    // }

    // //swap
    // {
    //     obs.clear();
    //     bool ok{false};
    //     boost::signals2::scoped_connection c =
    //         obs().on_insert(
    //             [&ok](obs_t::const_iterator)
    //             {
    //                 ok = true;
    //             });
    //     obs_t other{{2, "abc"}, {3, "def"}};
    //     obs().swap(other);
    //     assert(ok);
    // }
    
    // //count
    // {
    //     obs.clear();
    //     obs().insert({ {2, "abc"}, {3, "def"} });
    //     assert(obs().count(2) == 1);
    //     assert(obs().count(0) == 0);
    // }
    
    // //find fail
    // {
    //     obs.clear();
    //     assert(obs().find(0) == obs().end());
    //     const auto& cobs = obs();
    //     assert(cobs.find(0) == obs().cend());
        
    // }
    
    // //find success
    // {
    //     obs.clear();
    //     obs.emplace(2, "abc");
    //     obs.emplace(3, "def");
    //     auto it = obs().find(2);
    //     assert(it != obs().end());
    //     auto ob = *it;
    //     // bool called{false};
    //     // ob.second->on_change([&called]()
    //     //                     { called = true; });
    //     auto it2 = obs().find(2);
    //     assert(it2 != obs().end());
    //     // auto ob2 = *it2;
    //     // ob2.second->assign("def");
    //     // assert(ob.second->get() == "def");
    //     // assert(called);        
    // }
    
    // //equal_range
    // {
    //     obs.clear();
    //     obs.emplace(2, "abc");
    //     obs.emplace(3, "def");
    //     assert(obs().equal_range(2).first != obs().end());
    //     assert(obs().equal_range(4).first == obs().end());
    // }

}
