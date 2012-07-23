#include <boost/variant.hpp>
#include <boost/date_time.hpp>
#include <boost/type_traits.hpp>
#include <boost/type_traits/is_arithmetic.hpp>
#include <boost/utility/enable_if.hpp>

#include <iostream>
#include <cstdint>
#include <sstream>

// types for dynamic documents
typedef boost::variant<
    int32_t,
    uint32_t,
    int64_t,
    uint64_t,
    double,
    bool,
    std::string,
    boost::posix_time::ptime> document_scalar;

typedef boost::make_recursive_variant< // will your eyes explode?
    document_scalar,
    std::vector<boost::recursive_variant_>,
    std::map<std::string, boost::recursive_variant_>
    > document;


typedef std::vector<document> document_array;
typedef std::map<std::string, document> document_map;



// static documents are simply c++ types

// conversion to json
std::string to_json(const std::string& s)
{
    return "\"" + s + "\"";
}

std::string to_json(const boost::posix_time::ptime& pt)
{
    return "\"" + to_iso_string(pt) + "\"";
}

template<typename T>
std::string to_json(const T& t, typename boost::enable_if<boost::is_arithmetic<T> >::type* dummy = nullptr)
{
    std::stringstream ss;
    ss << t;
    return ss.str();
}

struct to_json_visitor : public boost::static_visitor<std::string>
{
    template<typename T>
    std::string operator()(const T& t) const
    {
        return to_json(t);
    }
};

std::string to_json(const document_scalar& scalar)
{
    return boost::apply_visitor(to_json_visitor(), scalar);
}


int main(int argc, char** argv)
{
    std::cout << "hello" << std::endl;

    uint32_t i = 223344;
    std::cout << "integer: " << to_json(i) << std::endl;

    double d = 3.14159;
    std::cout << "double: " << to_json(d) << std::endl;

    std::string s("bububaba");
    std::cout << "string: " << to_json(s) << std::endl;

    document_scalar ds(3.1415);
    std::cout << "scalar with double: " << to_json(ds) << std::endl;

    document_scalar dt(boost::posix_time::second_clock::local_time());
    std::cout << "scalar with ptime: " << to_json(dt) << std::endl;
}
