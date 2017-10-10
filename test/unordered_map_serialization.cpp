#include <observable/unordered_map.hpp>
#include <observable/value.hpp>
#include <observable/serialization/unordered_map.hpp>
#include <observable/serialization/value.hpp>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <fstream>
#include <string>

using map_t = observable::unordered_map<
    std::string,
    int
>;

using omap_t = observable::unordered_map<
    std::string,
    observable::value<int>
>;

template<typename ocontainer, typename scontainer>
void check_equal(ocontainer& c, scontainer&& sc)
{
    assert(c.size() == sc.size());
    for(typename ocontainer::reference e : c)
        assert(e.second == sc.at(e.first));
}

int main()
{
    std::string file("/tmp/observable_unordered_map_serialization");
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
        check_equal(map,
            std::unordered_map<map_t::key_type, map_t::mapped_type>{
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
        check_equal(map, 
            std::unordered_map<map_t::key_type, map_t::mapped_type>{
            {"k1", 1},
            {"k2", 2},
            {"k3", 3},
        });        
    }    
}
