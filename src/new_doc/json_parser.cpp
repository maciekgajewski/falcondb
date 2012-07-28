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

#include "new_doc/json_parser.hpp"

#include "utils/exception.hpp"

#include <boost/optional.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

#define UU qi::unused_type, qi::unused_type

namespace falcondb {

namespace qi = boost::spirit::qi;
namespace phoenix = boost::phoenix;

namespace detail {

template<typename iterator_type>
class scalar_parser : public qi::grammar<iterator_type, document_scalar()>
{
public:
    scalar_parser() : scalar_parser::base_type(_start)
    {

        _escape_chars.add("\\\"", '"')("\\\\", '\\');
        _double_quoted_string =
            qi::lit('"') >>
            *(_escape_chars | (qi::char_ - '\\' - '"')) >>
            '"';

        _start %=
            ( _double_quoted_string
            | qi::uint_parser<std::uint32_t>()
            | qi::int_parser<std::int32_t>()
            | qi::uint_parser<std::uint64_t>()
            | qi::int_parser<std::int64_t>()
            | qi::double_
            | qi::bool_
            );

    }
private:
    qi::symbols<char, char> _escape_chars;
    qi::rule<std::string::const_iterator, std::string()> _double_quoted_string;
    qi::rule<iterator_type, document_scalar()> _start;
};

template<typename iterator_type>
class document_parser : public qi::grammar<iterator_type, document(), qi::ascii::space_type>
{
public:
    document_parser() : document_parser::base_type(_document)
    {
        using namespace qi::labels;
        using phoenix::push_back;

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
        /*
        _pair =
            _double_quoted_string[ boost::phoenix::at_c<0>(qi::labels::_val) = boost::phoenix::argument<0>()]
            >> ':'
            >> _document[ boost::phoenix::at_c<1>(qi::labels::_val) = boost::phoenix::argument<0>()];
        _map = '{' >> _pair % ',' >> '}';
        */

        _document = _scalar | _array /*| _map*/;
    }

private:
    qi::symbols<char, char> _escape_chars;
    qi::rule<std::string::const_iterator, std::string()> _double_quoted_string;
    qi::rule<iterator_type, document_scalar()> _scalar;
    qi::rule<iterator_type, document_array(), qi::ascii::space_type> _array;
    qi::rule<iterator_type, std::pair<std::string, document>(), qi::ascii::space_type> _pair;
    qi::rule<iterator_type, document_map(), qi::ascii::space_type> _map;
    qi::rule<iterator_type, document(), qi::ascii::space_type> _document;

};

}

json_parser::json_parser()
{
}

document_scalar json_parser::parse(const std::string& in)
{
    std::string::const_iterator first = in.begin();
    std::string::const_iterator last = in.end();

    detail::scalar_parser<std::string::const_iterator> scalar_parser;

    boost::optional<document_scalar> result;
    qi::parse(
        first,
        last,
        scalar_parser[ [&result](const document_scalar& r, UU) { result = r; } ]
    );

    if (first != last)
    {
        throw exception("error at: ",  std::string(first, last));
    }

    return *result;
}

document json_parser::parse_doc(const std::string& in)
{
    std::string::const_iterator first = in.begin();
    std::string::const_iterator last = in.end();

    detail::document_parser<std::string::const_iterator> doc_parser;

    boost::optional<document> result;

    qi::phrase_parse(
        first,
        last,
        doc_parser[ [&result](const document& r, UU) { result = r; } ],
        qi::ascii::space
    );

    if (first != last)
    {
        throw exception("error at: ",  std::string(first, last));
    }

    return *result;
}

}
