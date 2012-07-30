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

#ifndef FALCONDB_DOCUMENT_OBJECT_IPP
#define FALCONDB_DOCUMENT_OBJECT_IPP

#include "document/document_object.hpp"
#include "document/document.hpp"

#include "utils/exception.hpp"

namespace falcondb {

inline
document_object::document_object(const detail::raw_document_map& map)
: detail::raw_document_map(map)
{ }

inline
document_object::document_object(detail::raw_document_map&& map)
: detail::raw_document_map(map)
{ }

document_object::document_object(const document& d)
: detail::raw_document_map(d.as_object())
{ }

document_object::document_object(document&& d)
: detail::raw_document_map(std::move(d.as_object()))
{ }


// check for field's existence
inline bool document_object::has_field(const std::string& field_name) const
{
    return find(field_name) != end();
}

// return or throw
inline const document& document_object::get_field(const std::string& field_name) const
{
    auto it = find(field_name);
    if (it != end())
    {
        return it->second;
    }
    else
    {
        throw exception("No such field '", field_name, "'");
    }
}

inline document& document_object::get_field(const std::string& field_name)
{
    auto it = find(field_name);
    if (it != end())
    {
        return it->second;
    }
    else
    {
        throw exception("No such field '", field_name, "'");
    }
}


template<typename T>
std::map<std::string, T> document_object::to_map_of() const
{
    std::map<std::string, T> result;
    std::transform(begin(), end(), std::inserter(result, result.end()),
        [](const value_type& p)
        {
            return std::make_pair(p.first, p.second.as<T>());
        });
    return result;
}

template<typename T>
document_object document_object::from(const std::map<std::string, T>& m)
{
    document_object result;
    std::transform(
        m.begin(), m.end(),
        std::inserter(result, result.end()),
        [] (const std::pair<std::string, T>& in)
        {
            return std::make_pair(in.first, document::from(in.second));
        });
    return result;
}

template<typename T>
void document_object::set_field(const std::string& field_name, const T& t)
{
    document d = document::from(t);
    auto r = insert(std::make_pair(field_name, d));
    if (!r.second)
    {
        r.first->second = d;
    }
}

template<typename T>
void document_object::set_field(const std::string& field_name, T&& t)
{
    document d = document::from(t);
    auto r = insert(std::make_pair(field_name, d));
    if (!r.second)
    {
        r.first->second = d;
    }
}

}

#endif
