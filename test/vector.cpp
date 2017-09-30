#include "observable/vector.hpp"

#include <string>
#include <vector>

using vector_t = std::vector<std::string>;
using ovector_t = observable::vector<std::string>;

template<typename ocontainer, typename value_type>
void check_equal(ocontainer& c, std::initializer_list<value_type> il)
{
    auto it = il.begin();
    assert(c.size() == il.size());
    for(auto e : c)
        assert(e == *it++);
}

int main()
{
    //ovector_t()
    {
        ovector_t ovec;        
    }

    //ovector_t(vector_t)
    {
        vector_t vec{"a", "b", "c"};
        ovector_t ovec(vec);
        check_equal(ovec, {"a", "b", "c"});
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
        ovec.emplace(ovec.begin(), "abc");
        try
        {
            auto ovalue = ovec.at(0);
            assert(ovalue == "abc");
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
        ovec.emplace(ovec.begin(), "abc");
        try
        {
            const auto& covec = ovec;
            auto covalue = covec.at(0);
            assert(covalue == "abc");
        }
        catch(const std::out_of_range&)
        { ok = false; }
        assert(ok);            
    }

    //operator[] 
    {
        ovector_t ovec;
        ovec.emplace(ovec.begin(), "abc");
        auto ovalue = ovec[0];
        assert(ovalue == "abc");
    }

    //operator[] const
    {
        ovector_t ovec;
        ovec.emplace(ovec.begin(), "abc");
        const auto& covec = ovec;
        assert(covec[0] == "abc");
    }
    
    //front()
    {
        ovector_t ovec;
        ovec.emplace(ovec.begin(), "abc");
        auto ovalue = ovec.front();
        assert(ovalue == "abc");
    }
    
    //front() const
    {
        ovector_t ovec;
        ovec.emplace(ovec.begin(), "abc");
        auto& covec = ovec;
        auto ovalue = covec.front();
        assert(ovalue == "abc");
    }
    
    //back()
    {
        ovector_t ovec;
        ovec.emplace(ovec.begin(), "abc");
        ovec.emplace(std::next(ovec.begin()), "def");
        auto ovalue = ovec.back();
        assert(ovalue == "def");
    }
    
    //back() const
    {
        ovector_t ovec;
        ovec.emplace(ovec.begin(), "abc");
        ovec.emplace(std::next(ovec.begin()), "def");
        const auto& covec = ovec;
        assert(covec.back() == "def");
    }
    
    //begin()
    {
        ovector_t ovec;
        ovec.emplace(ovec.begin(), "abc");
        auto ovalue = *ovec.begin();
        assert(ovalue == "abc");
    }

    //cbegin()
    {
        ovector_t ovec;
        ovec.emplace(ovec.begin(), "abc");
        auto ovalue = *ovec.cbegin();
        assert(ovalue == "abc");
    }
    
    //end/cend()
    {
        ovector_t ovec;
        ovec.emplace(ovec.begin(), "abc");
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
        ovec.emplace_back("first");
        ovec.emplace(ovec.begin(), "abc");
        assert(ovec.size() == 2);
    }
    
    //max_size()
    {
        ovector_t ovec;
        ovec.emplace_back("abc");
        ovec.emplace_back("def");
        assert(ovec.max_size() >= 2);
    }
    
     //clear()
    {
        ovector_t ovec;
        ovec.emplace_back("abc");
        ovec.emplace_back("def");
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
        ovec.emplace_back("abc");
        assert(ovec.capacity() >= 1);
    }

    //shrink_to_fit()
    {
        ovector_t ovec;
        ovec.emplace_back("abc");
        ovec.shrink_to_fit();
    }
    
    //erase()
    {
        ovector_t ovec;
        ovec.emplace_back("abc");
        ovec.emplace_back("def");
        bool called{false};
        bool before_called{false};
        boost::signals2::scoped_connection c1 =
        ovec.before_erase([&before_called](ovector_t::const_iterator it)
                         {
                             before_called = true;
                             assert(*it == "abc");
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
        ovec.emplace_back("abc");
        ovec.emplace_back("def");
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
        ovec.emplace_back("abc");
        ovec.emplace_back("def");
        assert(called);
        assert(*ovec.cbegin() == "abc");
        assert(*std::next(ovec.cbegin()) == "def");
    }
    
    //push_back()
    {
        ovector_t ovec;
        bool called{false};
        ovec.emplace_back("abc");
        boost::signals2::scoped_connection c =
            ovec.on_insert([&called](ovector_t::const_iterator it)
                           {
                               assert(*it == "def");
                               called = true;
                           });
        ovec.emplace_back("def");
        assert(called);
        assert(*ovec.cbegin() == "abc");
        assert(*std::next(ovec.cbegin()) == "def");
    }
    
    //emplace_back()
    {
        ovector_t ovec;
        bool called{false};
        ovec.emplace_back("abc");
        boost::signals2::scoped_connection c =
            ovec.on_insert([&called](ovector_t::const_iterator it)
                           {
                               assert(*it == "def");
                               called = true;
                           });
        ovec.emplace_back("def");
        assert(called);
        assert(*ovec.cbegin() == "abc");
        assert(*std::next(ovec.cbegin()) == "def");
    }
    
    //pop_back()
    {
        ovector_t ovec;
        bool called{false};
        bool before_called{false};
        ovec.emplace_back("abc");
        boost::signals2::scoped_connection c1 =
        ovec.before_erase([&before_called](ovector_t::const_iterator it)
                         {
                             before_called = true;
                             assert(*it == "abc");
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
        assert(*ovec.cbegin() == "abc");
        assert(*std::next(ovec.cbegin()) == "def");
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
        assert(*ovec.cbegin() == "abc");
        assert(*std::next(ovec.cbegin()) == "def");
    }
    
    //insert(pos, count, const value_type&)
    {
        ovector_t ovec;
        bool called{false};
        ovec.on_insert([&called](ovector_t::const_iterator)
                       { called = true; });
        auto it = ovec.begin();
        ovec.insert(it, 2, "abc");
        assert(called);
        assert(*std::next(ovec.cbegin()) == "abc");
    }
    
    //insert(pos, first, last)
    {
        ovector_t ovec;
        bool called{false};
        ovec.on_insert([&called](ovector_t::const_iterator)
                       { called = true; });
        std::array<std::string, 2> a{{"abc", "def"}};
        ovec.insert(ovec.begin(), a.cbegin(), a.cend());
        assert(called);
        assert(*ovec.cbegin() == "abc");
        assert(*std::next(ovec.cbegin()) == "def");
    }

    //insert(ilist)
    {
        ovector_t ovec;
        bool called{false};
        ovec.on_insert([&called](ovector_t::const_iterator)
                       { called = true; });
        ovec.insert(ovec.begin(), {std::string{"abc"}, std::string{"def"}});
        assert(called);
        assert(*ovec.cbegin() == "abc");
        assert(*std::next(ovec.cbegin()) == "def");
    }

}
