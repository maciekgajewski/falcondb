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

#ifndef FALCONDB_DOCUMENT_HPP
#define FALCONDB_DOCUMENT_HPP

#include "utils/exception.hpp"

#include <jsoncpp/json/json.h>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <vector>
#include <iostream>

namespace falcondb {

/// Wrapper around document library
/// We need only very minimalistic set of operation to perform on a document,
/// this makes easy to create an abstraction to hide the actual implementation behind.
class document
{
public:

    /// creates null document
    document() : _internal() {}
    ~document() { }

    document(const document& other)
    : _internal(other._internal)
    { }

    document(document&& rvalue)
    {
        _internal.swap(rvalue._internal);
    }

    // field access

    bool has_field(const std::string& name) const
    {
        return _internal.isMember(name);
    }

    template<typename T>
    T get(const std::string& field_name) const;

    // field manipulation
    template<typename T>
    void append(const std::string& field_name, const T& value)
    {
        _internal[field_name] = value; // rely on the existence of compatible constructor
    }

    // other

    bool is_null() const
    {
        _internal.isNull();
    }

    // json i/o

    std::string to_json() const
    {
        Json::FastWriter writer;
        return writer.write(_internal);
    }

    static document from_json(const std::string& json_data)
    {
        Json::Reader reader;
        Json::Value val;
        if (!reader.parse(json_data, val, false))
        {
            throw exception("Error parsing json: ", reader.getFormattedErrorMessages());
        }

        return document(std::move(val));
    }

    // storage i/o

    std::string to_storage() const
    {
        return to_json();
    }

    static document from_storage(const std::string& storage_data)
    {
        return from_json(storage_data);
    }

private:

    document(const Json::Value& internal) : _internal(internal) { }
    document(Json::Value&& internal)
    {
        _internal.swap(internal);
    }

    friend std::ostream& operator << (std::ostream& s, const document& d);

    void boo();

    Json::Value _internal;
};

typedef std::vector<document> document_list;

inline std::ostream& operator << (std::ostream& s, const document& d)
{
    Json::FastWriter writer;
    return s << writer.write(d._internal);
}

template<>
inline document document::get<document>(const std::string& field_name) const
{
    if(has_field(field_name))
    {
        return document(_internal.get(field_name, Json::Value()));
    }
    else
    {
        throw exception("no filed named ", field_name);
    }
}

template<>
inline void document::append<boost::uuids::uuid>(const std::string& field_name, const boost::uuids::uuid& value)
{
    _internal[field_name] = to_string(value);
}

} // namespace falcondb

#endif // FALCONDB_DOCUMENT_HPP
