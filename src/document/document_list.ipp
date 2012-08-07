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

#ifndef FALCONDB_DOCUMENT_LIST_IPP
#define FALCONDB_DOCUMENT_LIST_IPP

#include "document/document_list.hpp"
#include "document/document.hpp"

namespace falcondb {

inline
document_list::document_list(const detail::raw_document_list& raw)
: detail::raw_document_list(raw)
{
}

inline
document_list::document_list(detail::raw_document_list&& raw)
: detail::raw_document_list(raw)
{
}

inline
document_list::document_list(std::initializer_list<document> initializer)
: detail::raw_document_list(initializer)
{
}


inline
document_list::document_list(const document& d)
: detail::raw_document_list(d.as_list())
{
}

inline
document_list::document_list(document&& d)
: detail::raw_document_list(d.as_list())
{
}

// collapse to std::vector of specific type
// requirement: there must be a compatible document::as<T>()
template<typename T>
std::vector<T> document_list::to_list_of() const
{
}

template<typename T>
document_list document_list::from(const std::vector<T>& input)
{
    document_list result;
    result.reserve(input.size());
    std::transform(input.begin(), input.end(), std::back_inserter(result),
        [](const T& i)
        {
            return document::from(i);
        });
    return result;
}


}

#endif
