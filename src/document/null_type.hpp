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

#ifndef FALCONDB_NULL_TYPE_HPP
#define FALCONDB_NULL_TYPE_HPP

#include <iostream>

namespace falcondb {

struct null_type { };

inline std::ostream& operator<<(std::ostream& o, const null_type&)
{
    return o << "null";
}

template<typename T>
bool operator < (const null_type&, const T&)
{
    return true; // null < everything
}

inline
bool operator == (const null_type&, const null_type&)
{
    return true; // all nulls are equal
}

}

#endif
