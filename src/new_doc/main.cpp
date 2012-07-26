#include <boost/type_traits.hpp>
#include <boost/type_traits/is_arithmetic.hpp>
#include <boost/utility/enable_if.hpp>

#include <iostream>
#include <sstream>
#include <tuple>

#include "new_doc/dynamic_document.hpp"
#include "new_doc/document_serializer.hpp"
#include "new_doc/json_document_writer.hpp"

template<typename T>
std::string to_json(const T& t)
{
    std::stringstream ss;
    falcondb::json_writer writer(ss);

    falcondb::write_document(writer, t);

    return ss.str();
}

template<typename T>
void test_output(const std::string& name, const T& v)
{
    std::cout << "testing " << name << ": " << std::endl;
    std::cout << "as json:" << std::endl;
    std::cout << to_json(v) << std::endl;
    std::cout << std::endl;
}

void scalars()
{
    std::uint32_t i = 223344;
    test_output("integer" , i);

    double d = 3.14159;
    test_output("double", d);

    std::string s("bububaba");
    test_output("string", s);

    falcondb::document_scalar ds(3.1415);
    test_output("scalar with double", ds);

    falcondb::document_scalar dt(boost::posix_time::second_clock::local_time());
    test_output("scalar with ptime", dt);

    test_output("strinbg literal", "hello");
}

void arrays()
{
    std::vector<int> vi = {1, 2, 3};
    test_output("vector<int>", vi);

    std::vector<std::string> vs = {"A", "b", "C"};
    test_output("vector<string>", vs);

    std::vector<falcondb::document_scalar> vds = {1, "b", boost::posix_time::second_clock::local_time()};
    test_output("vector<document_scalar>", vds);

    // tuple
    auto ti = std::make_tuple(1, 2, 3, 4);
    test_output("tuple of ints", ti);
}

void maps()
{
    // uniform map
    std::map<std::string, int> um = {std::make_pair("a", 1), std::make_pair("b", 2)};
    test_output("uniform map a:1, b:2", um);

    // variant scalar map
    std::map<std::string, falcondb::document_scalar> dsm =
        {std::make_pair("string", "hello"), std::make_pair("int", 7)};
    test_output("scalar map string:hello, int:7", dsm);
}

int main(int argc, char** argv)
{
    std::cout << "scalars" << std::endl << std::endl;
    scalars();

    std::cout << "arrays" << std::endl << std::endl;
    arrays();

    std::cout << "maps" << std::endl << std::endl;
    maps();
}
