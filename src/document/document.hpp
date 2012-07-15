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

#include "3rdparty/libjson/libjson.h"

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
    document() : _node() {}
    ~document() { }

    document(const document& other)
    : _node(other._node)
    { }

    document(document&& rvalue)
    {
        _node.swap(rvalue._node);
    }

    // field access

    bool has_field(const std::string& name) const
    {
        return _node.find(name) != _node.end();
    }

    template<typename T>
    T get(const std::string& field_name) const
    {
        return (T)_node.at(field_name);
    }

    // field manipulation
    template<typename T>
    void append(const std::string& field_name, const T& value)
    {
        _node.push_back(JSONNode(field_name, value));
    }

    // json i/o

    std::string to_json() const
    {
        return _node.write();
    }

    static document from_json(const std::string& json_data)
    {
        return document(libjson::parse(json_data));
    }

    // storage i/o

    std::string to_storage() const { return to_json(); }
    static document from_storage(const std::string& storage_data)
    {
        return from_json(storage_data);
    }

private:

    document(const JSONNode& node) : _node(node) { }

    void boo();

    JSONNode _node;
};

typedef std::vector<document> document_list;

inline std::ostream& operator << (std::ostream& s, const document& d)
{
    return s << d.to_json();
}

template<>
inline void document::append<boost::uuids::uuid>(const std::string& field_name, const boost::uuids::uuid& value)
{
    _node.push_back(JSONNode(field_name, to_string(value)));
}

} // namespace falcondb

#endif // FALCONDB_DOCUMENT_HPP
