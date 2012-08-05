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

#ifndef FALCONDB_DOCUMENT_OBJECT_HPP
#define FALCONDB_DOCUMENT_OBJECT_HPP

#include "document/detail/variants.hpp"

#include <map>

namespace falcondb {

/// Provides some extra functionality on top of document_map
class document_object : public detail::raw_document_map
{
public:

    document_object() = default; // wooohoo!
    document_object(const detail::raw_document_map& map);
    document_object(detail::raw_document_map&& map);
    document_object(const document& d);
    document_object(document&& d);

    document_object& operator = (const document_object& d) = default;

    // check for field's existence
    bool has_field(const std::string& field_name) const;

    // return or throw
    const document& get_field(const std::string& field_name) const;
    document& get_field(const std::string& field_name);

    // upsert
    template<typename T>
    void set_field(const std::string& field_name, const T& t);
    template<typename T>
    void set_field(const std::string& field_name, T&& t);

    // collapse to std::map of specific type
    // requirement: there must be a compatible document::as<T>()
    template<typename T>
    std::map<std::string, T> to_map_of() const;

    // converts from any map
    // requirement: there must be a compatible document::from<T>()
    template<typename T>
    static document_object from(const std::map<std::string, T>& m);

    // io
    std::string to_json() const;
    static document_object from_json(const std::string& s);

};

}

#endif
