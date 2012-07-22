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

#include "document/document.hpp"

namespace falcondb {

document::const_iterator document::insert(const document::const_iterator& pos, document& element)
{
    // implement the operation in terms of std::vector insert.
    // TODO the array-document should simply be implemented as vector
    std::vector<document> as_vector;
    as_vector.reserve(size()+1);
    for( std::size_t i = 0; i < size(); ++i)
    {
        as_vector.push_back((*this)[i]); // the original Json::Value iterators are useless
    }

    std::vector<document>::iterator vector_pos = as_vector.begin() + pos._index;
    as_vector.insert(vector_pos, element);

    swap(from_array(as_vector));

    return document_const_iterator(this, pos._index);
}

bool document::operator<(const document& other) const
{
    return _internal < other._internal;
}

} // namespace falcondb
