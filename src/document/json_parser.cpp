/*
FalconDB, a database
Copyright (C) 2012 Maciej Gajewski <maciej.gajewski0 at gmail dot com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "document/json_parser.hpp"

#include "utils/exception.hpp"

#include <boost/optional.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant/static_visitor.hpp>

#define UU qi::unused_type, qi::unused_type

namespace falcondb {

namespace qi = boost::spirit::qi;
namespace phoenix = boost::phoenix;
namespace fusion = boost::fusion;

namespace detail {

// modified variant which can be parsed
typedef boost::variant<
    std::string,
    std::int64_t,
    std::uint64_t,
    double,
    bool> parse_scalar;

class parse_pair;

typedef boost::make_recursive_variant< // will your eyes explode?
    parse_scalar,
    std::vector<boost::recursive_variant_>,
    std::vector<parse_pair>
    >::type parse_document;

// for some reason std::pair doesn't work and fusion::vector makes some issues
struct parse_pair
{
    std::string name;
    parse_document value;
};


typedef std::vector<parse_document> parse_array;
typedef std::vector<parse_pair> parse_map;


template<typename iterator_type>
class document_parser : public qi::grammar<iterator_type, parse_document(), qi::ascii::space_type>
{
public:
    document_parser() : document_parser::base_type(_document)
    {
        using namespace qi::labels;
        using phoenix::push_back;
        using phoenix::at_c;

        _escape_chars.add("\\\"", '"')("\\\\", '\\');
        _double_quoted_string =
            qi::lit('"') >>
            *(_escape_chars | (qi::char_ - '\\' - '"')) >>
            '"';

        _scalar =
            ( _double_quoted_string
            | qi::double_
            | qi::uint_parser<std::uint64_t>()
            | qi::int_parser<std::int64_t>()
            | qi::bool_
            );

        _array = '[' >> _document[push_back(_val, _1)] % ',' >> ']';
        _pair =
            _double_quoted_string
            >> ':'
            >> _document;
        _map = '{' >> _pair % ',' >> '}';

        _document = _scalar | _array | _map;
    }

private:


    qi::symbols<char, char> _escape_chars;
    qi::rule<std::string::const_iterator, std::string()> _double_quoted_string;
    qi::rule<iterator_type, parse_scalar()> _scalar;
    qi::rule<iterator_type, parse_array(), qi::ascii::space_type> _array;
    qi::rule<iterator_type, parse_pair(), qi::ascii::space_type> _pair;
    qi::rule<iterator_type, parse_map(), qi::ascii::space_type> _map;
    qi::rule<iterator_type, parse_document(), qi::ascii::space_type> _document;

};

// conversion from parser documents to falcon documents

raw_document_any convert_from_parser(const parse_document& );

inline raw_document_scalar convert_from_parser(const parse_scalar& s)
{
    return s; // variant automagic
}

inline document_list convert_from_parser(const parse_array& a)
{
    document_list result;
    result.reserve(a.size());
    std::transform(
        a.begin(), a.end(),
        std::back_inserter(result),
        [](const parse_document& d) { return convert_from_parser(d); }
        );
    return result;
}

inline document_object convert_from_parser(const parse_map& m)
{
    document_object result;
    for( auto p : m )
    {
        result.insert(std::make_pair(p.name, convert_from_parser(p.value)));
    }
    return result;
}

struct converter_from_parser : boost::static_visitor<raw_document_any>
{
    template<typename T>
    raw_document_any operator() (const T& t) const { return convert_from_parser(t); }
};

inline raw_document_any convert_from_parser(const parse_document& d)
{
    return boost::apply_visitor(converter_from_parser(), d);
}

}}

// this needs to be in a global namespace
BOOST_FUSION_ADAPT_STRUCT(
    falcondb::detail::parse_pair,
    (std::string, name)
    (falcondb::detail::parse_document, value)
)


namespace falcondb {

document json_parser::parse_doc(const std::string& in)
{
    std::string::const_iterator first = in.begin();
    std::string::const_iterator last = in.end();

    detail::document_parser<std::string::const_iterator> doc_parser;

    boost::optional<detail::parse_document> result;

    qi::phrase_parse(
        first,
        last,
        doc_parser[ [&result](const detail::parse_document& r, UU) { result = r; } ],
        qi::ascii::space
    );

    if (first != last)
    {
        throw exception("error at: ",  std::string(first, last));
    }

    return detail::convert_from_parser(*result);
}

}
