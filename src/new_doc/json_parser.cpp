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

#include <boost/spirit/include/qi.hpp>

namespace falcondb {

json_parser::json_parser()
{
}

document_scalar json_parser::parse(const std::string& in)
{
    using namespace boost::spirit;
    std::string::const_iterator first = in.begin();
    std::string::const_iterator last = in.end();
    document_scalar out(false);

    #define UU qi::unused_type, qi::unused_type

    qi::symbols<char, char> escape_chars;
    escape_chars.add("\\\"", '"');
    escape_chars.add("\\\\", '\\');

    qi::rule<std::string::const_iterator, std::string()> double_quoted_string
        = qi::lit('"') >>
            *(escape_chars | (qi::char_ - '\\' - '"')) >>
        '"';

    qi::parse(
        first,
        last,
            qi::uint_[ [&](unsigned int u, UU){ out = u; } ]
            |
            qi::int_[ [&](int i, UU){ out = i; } ]
            |
            qi::double_[ [&](double d, UU){ out = d; }  ]
            |
            double_quoted_string[ [&](const std::string& s, UU){ out = s; } ]
        );
    if (first != last)
    {
        throw exception("error at: ",  std::string(first, last));
    }

    return out;
}

}
