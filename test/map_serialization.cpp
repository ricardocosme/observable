#include <observable/exp_map.hpp>
#include <observable/value.hpp>
#include <observable/serialization/map.hpp>
#include <observable/serialization/value.hpp>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <fstream>
#include <string>

using map_t = observable::exp::map<
    std::string,
    int
>;

using omap_t = observable::exp::map<
    std::string,
    observable::value<int>
>;

template<typename ocontainer>
void check_equal(ocontainer& c,
                 std::initializer_list<typename ocontainer::value_type> il)
{
    auto it = il.begin();
    assert(c.size() == il.size());
    for(typename ocontainer::reference e : c)
        assert(e == *it++);
}

int main()
{
    std::string file("/tmp/observable_map_serialization");
    {
        map_t map{
            {"k1", 1},
            {"k2", 2},
            {"k3", 3},
        };
        std::ofstream out(file);
        boost::archive::text_oarchive oa(out);
        oa << map;
    }

    {
        map_t map;
        std::ifstream in(file);
        boost::archive::text_iarchive ia(in);
        ia >> map;
        check_equal(map, {
            {"k1", 1},
            {"k2", 2},
            {"k3", 3},
        });        
    }    

    {
        omap_t map;
        map.emplace("k1", 1);
        map.emplace("k2", 2);
        map.emplace("k3", 3);
        std::ofstream out(file);
        boost::archive::text_oarchive oa(out);
        oa << map;
    }

    {
        omap_t map;
        std::ifstream in(file);
        boost::archive::text_iarchive ia(in);
        ia >> map;
        check_equal(map, {
            {"k1", 1},
            {"k2", 2},
            {"k3", 3},
        });        
    }    
}
