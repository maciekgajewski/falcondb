#include "bson/bsonobjbuilder.hpp"

#include "document/document.hpp"
#include "document/json_writer.hpp"
#include "document/json_parser.hpp"

#include <boost/type_traits.hpp>
#include <boost/type_traits/is_arithmetic.hpp>
#include <boost/utility/enable_if.hpp>

#include <iostream>
#include <sstream>
#include <tuple>

#include <ctime>


template<typename T>
std::string to_json(const T& t)
{
    std::stringstream ss;
    falcondb::json_writer writer(ss);

    writer.write(t);

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

    falcondb::raw_document_scalar ds(3.1415);
    test_output("scalar with double", ds);

    falcondb::raw_document_scalar dt(boost::posix_time::second_clock::local_time());
    test_output("scalar with ptime", dt);

    test_output("string literal", "hello");
}

void arrays()
{
    std::vector<int> vi = {1, 2, 3};
    test_output("vector<int>", vi);

    std::vector<std::string> vs = {"A", "b", "C"};
    test_output("vector<string>", vs);

    std::vector<falcondb::raw_document_scalar> vds = {1, "b", boost::posix_time::second_clock::local_time()};
    test_output("vector<document_scalar>", vds);

    // tuple
    //auto ti = std::make_tuple(1, 2, 3, 4);
    //test_output("tuple of ints", ti);
}

void maps()
{
    // uniform map
    std::map<std::string, int> um = {std::make_pair("a", 1), std::make_pair("b", 2)};
    test_output("uniform map a:1, b:2", um);

    // variant scalar map
    std::map<std::string, falcondb::raw_document_scalar> dsm =
        {std::make_pair("string", "hello"), std::make_pair("int", 7)};
    test_output("scalar map string:hello, int:7", dsm);
}

void nested()
{
    // array of maps
    typedef std::map<std::string, double> md_t;
    typedef std::vector<md_t> mdv_t;

    mdv_t mdv = { {std::make_pair("pi", 3.1415), std::make_pair("e", 2.718)},
        {std::make_pair("wieght", 124.2), std::make_pair("height", 200)} };
    test_output("vector of maps of doubles", mdv);

    // map of arrays
    typedef std::vector<bool> vb_t;
    typedef std::map<std::string, vb_t> vbm_t;

    vbm_t vbm = { std::make_pair("boolseq1", vb_t{true, false, true, false}), std::make_pair("boolseq2", vb_t{true, true}) };
    test_output("map of vectors of bools", vbm);
}

void document()
{
    falcondb::document dsi = falcondb::document::from(3);
    test_output("document with scalar", dsi);

    std::vector<falcondb::raw_document_scalar> vds = {1, "b", boost::posix_time::second_clock::local_time()};
    //falcondb::document dav = falcondb::to_document_list(vds.begin(), vds.end());
    falcondb::document dav = falcondb::document::from(vds);
    test_output("document with variant array", dav);

    // map of arrays
    typedef std::vector<bool> vb_t;
    typedef std::map<std::string, vb_t> vbm_t;

    vbm_t vbm = { std::make_pair("boolseq1", vb_t{true, false, true, false}), std::make_pair("boolseq2", vb_t{true, true}) };
    //falcondb::document vbmd = falcondb::to_document_object(vbm.begin(), vbm.end());
    falcondb::document vbmd = falcondb::document::from(vbm);
    test_output("document with map of vectors of bools", vbmd);
}

void test_parse_scalar(const std::string& input)
{
    try
    {
        falcondb::raw_document_scalar res = falcondb::json_parser::parse_doc(input).as_scalar();
        std::cout << input << " => " << res << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cout << "error parsing " << input << " : " << e.what() << std::endl;
    }
    std::cout << std::endl;
}

void hobbit(const std::string& json) // there and back again
{
    std::cout << "hobbit: " << std::endl;
    try
    {
        falcondb::document d = falcondb::json_parser::parse_doc(json);
        std::string back = to_json(d);

        std::cout << json << " -> " << back << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cout << "parse error: " << e.what() << std::endl;
    }
    std::cout << std::endl;
}

int main(int argc, char** argv)
{
    std::cout << "scalars" << std::endl << std::endl;
    scalars();

    std::cout << "arrays" << std::endl << std::endl;
    arrays();

    std::cout << "maps" << std::endl << std::endl;
    maps();

    std::cout << "nested types" << std::endl << std::endl;
    nested();

    std::cout << "full document" << std::endl << std::endl;
    document();

    // let's have some bson!
    mongo::BSONObjBuilder builder;
    builder.append("string", "bubububu");
    builder.append("double", 3.1415);
    builder.appendArray("array", mongo::BSONArrayBuilder().append(1).append(2).append(3).arr());
    builder.append("date", mongo::Date_t(std::time(NULL)));

    mongo::BSONObj o = builder.obj();

    std::string as_json = o.jsonString();
    std::cout << "bson as json: " << as_json << std::endl;

    // parser
    test_parse_scalar("123.45");
    test_parse_scalar("123.45dfgdf");
    test_parse_scalar("what?");
    test_parse_scalar("55");
    test_parse_scalar("-55");
    test_parse_scalar("\"valid string\"");
    test_parse_scalar("\"invalid string with\"quote\"");
    test_parse_scalar("\"invalid string with\\backslash\"");
    test_parse_scalar("\"valid string with\\\\backslash\"");
    test_parse_scalar("\"valid string with\\\"quote\"");
    test_parse_scalar("true");
    test_parse_scalar("false");
    test_parse_scalar("100000000000");

    hobbit("[1,2,3]");
    hobbit("[ 1 , 2 , 3 ]");
    hobbit("\"moo\"");
    hobbit("12443");
    hobbit("12443.55");
    hobbit("[\"moo\", \"moo\", \"moo\"]");
    hobbit("[\"moo\", 23.45, true]");
    hobbit("[\"moo\", 23.45, true, [1, 2, \"no!\"]]");
    hobbit(" { \"ala\" : \"ma kota\", \"heniek\" : \"ma kaca\" } ");
    hobbit(" { \"constants\" : { \"pi\":3.14,\"e\":2.7 } , \"natual\":[1,2,3,4,5,6,7,8,9] } ");


}
