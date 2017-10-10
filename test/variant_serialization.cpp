#include <observable/variant.hpp>
#include <observable/value.hpp>
#include <observable/serialization/variant.hpp>
#include <observable/serialization/value.hpp>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <fstream>
#include <string>

using variant_t = observable::variant<
    int,
    std::string
>;

int main()
{
    std::string file("/tmp/observable_variant_serialization");
    {
        variant_t variant;
        variant = "abc";
        std::ofstream out(file);
        boost::archive::text_oarchive oa(out);
        oa << variant;
    }

    {
        variant_t variant;
        std::ifstream in(file);
        boost::archive::text_iarchive ia(in);
        ia >> variant;
        assert(boost::get<std::string>(variant.model()) == "abc");
    }    

    {
        variant_t variant;
        variant = 10;
        std::ofstream out(file);
        boost::archive::text_oarchive oa(out);
        oa << variant;
    }

    {
        variant_t variant;
        std::ifstream in(file);
        boost::archive::text_iarchive ia(in);
        ia >> variant;
        assert(boost::get<int>(variant.model()) == 10);
    }

    {
        using type = observable::variant<int, observable::value<std::string>>;
        type variant;
        variant = std::string{"abc"};
        {
            std::ofstream out(file);
            boost::archive::text_oarchive oa(out);
            oa << variant;
        }
        std::ifstream in(file);
        boost::archive::text_iarchive ia(in);
        ia >> variant;
        assert(boost::get<observable::value<std::string>>(variant.model()) == std::string{"abc"});
    }
}
