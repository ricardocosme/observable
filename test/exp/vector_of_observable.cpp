#include "observable/exp/value.hpp"
#include "observable/exp/vector.hpp"

#include <string>
#include <vector>

using vector_t = std::vector<std::string>;
using ovector_t = observable::exp::vector<observable::exp::value<std::string>>;

template<typename ocontainer, typename value_type>
void check_equal(ocontainer& c, std::initializer_list<value_type> il)
{
    auto it = il.begin();
    assert(c.size() == il.size());
    for(auto& e : c)
        assert(e.get() == *it++);
}

int main()
{
    //ovector_t()
    {
        ovector_t ovec;        
    }

    //ovector_t(vector_t)
    {
        ovector_t ovec;
        ovec.emplace_back(std::string{"a"});
        ovec.emplace_back(std::string{"b"});
        ovec.emplace_back(std::string{"c"});
        check_equal(ovec, {"a", "b", "c"});        
    }
    
    //ovector_t.on_value_change()
    {
        ovector_t ovec;
        ovec.emplace_back(std::string{"a"});
        bool called{false};
        ovec.on_value_change
            ([&called](ovector_t::const_iterator it)
             {
                 assert(it->get() == "change");
                 called = true;
             });
        ovec.front() = "change";
        assert(called);
    }
    
    //ovector_t.on_value_change()
    {
        ovector_t ovec;
        ovec.emplace_back(std::string{"a"});
        bool called{false};
        ovec.on_value_change
            ([&called](ovector_t::const_iterator it)
             {
                 assert(it->get() == "change");
                 called = true;
             });
        ovec.emplace_back(std::string{"b"});
        ovec.front() = "change";
        assert(called);
    }
    
    //ovector_t.on_value_change()
    {
        ovector_t ovec;
        ovec.emplace_back(std::string{"a"});
        bool called{false};
        ovec.on_value_change
            ([&called](ovector_t::const_iterator it)
             {
                 assert(it->get() == "change");
                 called = true;
             });
        ovec.emplace_back(std::string{"b"});
        auto ovec2 = std::move(ovec);        
        ovec2.front() = "change";
        assert(called);
    }
    
    //ovector_t.on_change()
    {
        ovector_t ovec;
        ovec.emplace_back(std::string{"a"});
        bool called{false};
        ovec.on_change
            ([&called]()
             {
                 called = true;
             });
        ovec.front() = "change";
        assert(called);
    }
        
    //at fail
    {
        ovector_t ovec;
        bool ok{false};
        try
        {
            ovec.at(0);
        }
        catch(const std::out_of_range&)
        { ok = true; }
        assert(ok);
    }

    //at success
    {
        bool ok{true};
        ovector_t ovec;
        ovec.emplace(ovec.begin(), std::string{"abc"});
        try
        {
            bool called{false};
            auto& ovalue = ovec.at(0);
            assert(ovalue.get() == "abc");
            ovalue.on_change([&called]()
                          { called = true; });
            ovalue.assign("def");
            assert(ovalue.get() == "def");
            assert(called);
        }
        catch(const std::out_of_range&)
        { ok = false; }
        assert(ok);
    }
    
    //at const fail
    {
        ovector_t ovec;
        bool ok{false};
        try
        {
            const auto& covalue = ovec;
            covalue.at(0);
        }
        catch(const std::out_of_range&)
        { ok = true; }
        assert(ok);
    }

    //at const success
    {
        bool ok{true};
        ovector_t ovec;
        ovec.emplace(ovec.begin(), std::string{"abc"});
        try
        {
            const auto& covec = ovec;
            auto& covalue = covec.at(0);
            assert(covalue.get() == "abc");
        }
        catch(const std::out_of_range&)
        { ok = false; }
        assert(ok);            
    }

    //operator[] 
    {
        ovector_t ovec;
        ovec.emplace(ovec.begin(), std::string{"abc"});
        bool called{false};
        auto& ovalue = ovec[0];
        assert(ovalue.get() == std::string{"abc"});
        ovalue.on_change([&called]()
                     { called = true; });
        ovalue.assign("def");
        assert(ovalue.get() == "def");
        assert(called);
    }

    //operator[] const
    {
        ovector_t ovec;
        ovec.emplace(ovec.begin(), std::string{"abc"});
        const auto& covec = ovec;
        assert(covec[0].get() == "abc");
    }
    
    //front()
    {
        ovector_t ovec;
        ovec.emplace(ovec.begin(), std::string{"abc"});
        auto& ovalue = ovec.front();
        assert(ovalue.get() == "abc");
        bool called{false};
        ovalue.on_change([&called]()
                         { called = true; });
        ovalue.assign("def");
        assert(ovalue.get() == "def");
        assert(called);
    }
    
    //front() const
    {
        ovector_t ovec;
        ovec.emplace(ovec.begin(), std::string{"abc"});
        auto& covec = ovec;
        auto& ovalue = covec.front();
        assert(ovalue.get() == "abc");
        bool called{false};
        ovalue.on_change([&called]()
                         { called = true; });
        ovalue.assign("def");
        assert(ovalue.get() == "def");
        assert(called);
    }
    
    //back()
    {
        ovector_t ovec;
        ovec.emplace(ovec.begin(), std::string{"abc"});
        ovec.emplace(std::next(ovec.begin()), std::string{"def"});
        auto& ovalue = ovec.back();
        assert(ovalue.get() == "def");
        bool called{false};
        ovalue.on_change([&called]()
                         { called = true; });
        ovalue.assign("ghi");
        assert(ovalue.get() == "ghi");
        assert(called);
    }
    
    //back() const
    {
        ovector_t ovec;
        ovec.emplace(ovec.begin(), std::string{"abc"});
        ovec.emplace(std::next(ovec.begin()), std::string{"def"});
        const auto& covec = ovec;
        assert(covec.back().get() == "def");
    }
    
    //begin()
    {
        ovector_t ovec;
        ovec.emplace(ovec.begin(), std::string{"abc"});
        auto& ovalue = *ovec.begin();
        assert(ovalue.get() == "abc");
        bool called{false};
        ovalue.on_change([&called]()
                         { called = true; });
        ovalue.assign("def");
        assert(ovalue.get() == "def");
        assert(called);
    }

    //cbegin()
    {
        ovector_t ovec;
        ovec.emplace(ovec.begin(), std::string{"abc"});
        auto& ovalue = *ovec.cbegin();
        assert(ovalue.get() == "abc");
    }
    
    //end/cend()
    {
        ovector_t ovec;
        ovec.emplace(ovec.begin(), std::string{"abc"});
        assert(ovec.begin() != ovec.end());
        assert(ovec.cbegin() != ovec.cend());
    }

    //empty()
    {
        ovector_t ovec;
        assert(ovec.empty());
    }
    
    //size()
    {
        ovector_t ovec;
        ovec.emplace_back(std::string{"first"});
        ovec.emplace(ovec.begin(), std::string{"abc"});
        assert(ovec.size() == 2);
    }
    
    //max_size()
    {
        ovector_t ovec;
        ovec.emplace_back(std::string{"abc"});
        ovec.emplace_back(std::string{"def"});
        assert(ovec.max_size() >= 2);
    }
    
     //clear()
    {
        ovector_t ovec;
        ovec.emplace_back(std::string{"abc"});
        ovec.emplace_back(std::string{"def"});
        bool called{false};
        bool before_called{false};
        boost::signals2::scoped_connection c1 =
            ovec.before_erase([&before_called, &ovec](ovector_t::const_iterator it)
                              {
                                  before_called = true;
                                  assert(ovec.cend() == it);
                              });
        boost::signals2::scoped_connection c2 =
            ovec.on_erase([&called](ovector_t::const_iterator)
                          { called = true; });
        ovec.clear();
        assert(before_called);
        assert(called);
    }
        
    //reserve()
    {
        ovector_t ovec;
        ovec.reserve(100);
    }
    
    //capacity()
    {
        ovector_t ovec;
        ovec.emplace_back(std::string{"abc"});
        assert(ovec.capacity() >= 1);
    }

    //shrink_to_fit()
    {
        ovector_t ovec;
        ovec.emplace_back(std::string{"abc"});
        ovec.shrink_to_fit();
    }
    
    //erase()
    {
        ovector_t ovec;
        ovec.emplace_back(std::string{"abc"});
        ovec.emplace_back(std::string{"def"});
        bool called{false};
        bool before_called{false};
        boost::signals2::scoped_connection c1 =
        ovec.before_erase([&before_called](ovector_t::const_iterator it)
                         {
                             before_called = true;
                             assert(it->get() == "abc");
                         });
        boost::signals2::scoped_connection c2 =
        ovec.on_erase([&called](ovector_t::const_iterator)
                      { called = true; });
        ovec.erase(ovec.begin());
        assert(before_called);
        assert(called);
        assert(ovec.size() == 1);
    }
    
    //erase(first, last)
    {
        ovector_t ovec;
        ovec.emplace_back(std::string{"abc"});
        ovec.emplace_back(std::string{"def"});
        bool called{false};
        bool before_called{false};
        boost::signals2::scoped_connection c1 =
        ovec.before_erase([&before_called, &ovec](ovector_t::const_iterator it)
                         {
                             before_called = true;
                             assert(ovec.begin() == it);
                         });
        boost::signals2::scoped_connection c2 =
        ovec.on_erase([&called](ovector_t::const_iterator)
                     { called = true; });
        ovec.erase(ovec.begin(), ovec.end());
        assert(before_called);
        assert(called);
        assert(ovec.empty());
    }
    
    //emplace()
    {
        ovector_t ovec;
        bool called{false};
        ovec.on_insert([&called](ovector_t::const_iterator)
                       { called = true; });
        ovec.emplace_back(std::string{"abc"});
        ovec.emplace_back(std::string{"def"});
        assert(called);
        assert(ovec.cbegin()->get() == "abc");
        assert(std::next(ovec.cbegin())->get() == "def");
    }
    
    //push_back()
    {
        ovector_t ovec;
        bool called{false};
        ovec.emplace_back(std::string{"abc"});
        boost::signals2::scoped_connection c =
            ovec.on_insert([&called](ovector_t::const_iterator it)
                           {
                               assert(it->get() == "def");
                               called = true;
                           });
        ovec.emplace_back(std::string{"def"});
        assert(called);
        assert(ovec.cbegin()->get() == "abc");
        assert(std::next(ovec.cbegin())->get() == "def");
    }
    
    //emplace_back()
    {
        ovector_t ovec;
        bool called{false};
        ovec.emplace_back(std::string{"abc"});
        boost::signals2::scoped_connection c =
            ovec.on_insert([&called](ovector_t::const_iterator it)
                           {
                               assert(it->get() == "def");
                               called = true;
                           });
        ovec.emplace_back(std::string{"def"});
        assert(called);
        assert(ovec.cbegin()->get() == "abc");
        assert(std::next(ovec.cbegin())->get() == "def");
    }
    
    //pop_back()
    {
        ovector_t ovec;
        bool called{false};
        bool before_called{false};
        ovec.emplace_back(std::string{"abc"});
        boost::signals2::scoped_connection c1 =
        ovec.before_erase([&before_called](ovector_t::const_iterator it)
                         {
                             before_called = true;
                             assert(it->get() == "abc");
                         });
        boost::signals2::scoped_connection c2 =
            ovec.on_erase([&called](ovector_t::const_iterator it)
                          {
                              called = true;
                          });
        ovec.pop_back();
        assert(before_called);
        assert(called);
        assert(ovec.empty());
    }
    
    //insert(pos, const value_type&)
    {
        ovector_t ovec;
        bool called{false};
        ovec.on_insert([&called](ovector_t::const_iterator)
                       { called = true; });
        std::string v = "abc";
        ovec.insert(ovec.begin(), v);
        std::string v2 = "def";
        ovec.insert(std::next(ovec.begin()), v2);
        assert(called);
        assert(ovec.cbegin()->get() == "abc");
        assert(std::next(ovec.cbegin())->get() == "def");
    }

    //insert(pos, value_type&&)
    {
        ovector_t ovec;
        bool called{false};
        ovec.on_insert([&called](ovector_t::const_iterator)
                       { called = true; });
        std::string v = "abc";
        ovec.insert(ovec.begin(), v);
        std::string v2 = "def";
        ovec.insert(std::next(ovec.begin()), v2);
        assert(called);
        assert(ovec.cbegin()->get() == "abc");
        assert(std::next(ovec.cbegin())->get() == "def");
    }
    
    // //insert(pos, count, const value_type&)
    // {
    //     ovector_t ovec;
    //     bool called{false};
    //     ovec.on_insert([&called](ovector_t::const_iterator)
    //                    { called = true; });
    //     auto it = ovec.begin();
    //     ovec.insert(it, 2, "abc");
    //     assert(called);
    //     assert(std::next(ovec.cbegin())->get() == "abc");
    // }
    
    //insert(pos, first, last)
    {
        ovector_t ovec;
        bool called{false};
        ovec.on_insert([&called](ovector_t::const_iterator)
                       { called = true; });
        std::array<std::string, 2> a{{"abc", "def"}};
        ovec.insert(ovec.begin(), a.cbegin(), a.cend());
        assert(called);
        assert(ovec.cbegin()->get() == "abc");
        assert(std::next(ovec.cbegin())->get() == "def");
    }

    // //insert(ilist)
    // {
    //     ovector_t ovec;
    //     bool called{false};
    //     ovec.on_insert([&called](ovector_t::const_iterator)
    //                    { called = true; });
    //     ovec.insert(ovec.begin(), {std::string{"abc"}, std::string{"def"}});
    //     assert(called);
    //     assert(ovec.cbegin()->get() == "abc");
    //     assert(std::next(ovec.cbegin())->get() == "def");
    // }

}
