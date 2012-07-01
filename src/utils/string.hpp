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

#ifndef FALCONDB_STRING_HPP
#define FALCONDB_STRING_HPP

#include <sstream>

namespace falcondb {

namespace detail {

template <typename A>
void build_string(std::ostream& s, const A& a)
{
    s << a;
}

template<typename A, typename ...B>
void build_string(std::ostream& s, const A& a, const B& ...b)
{
    s << a;
    build_string(s, b...);
}

}

/// concatenates all params into string
template<typename ...T>
std::string build_string(const T& ...t)
{
    std::ostringstream ss;
    detail::build_string(ss, t...);
    return ss.str();
}

}

#endif
