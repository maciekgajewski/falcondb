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

class document;

/// Document iterator, used to iterate array document
class document_const_iterator
{
public:
    document_const_iterator(const document* parent, std::size_t index)
    : _parent(parent), _index(index)
    { }

    typedef document value_type;
    typedef std::size_t difference_type;
    typedef document* pointer;
    typedef document& reference;
    typedef std::random_access_iterator_tag iterator_category;

    document operator*();

    //void operator=(const document_const_iterator& other);

    document_const_iterator& operator ++ () { _index++; return *this; }
    void operator += (difference_type d) { _index += d; }
    difference_type operator-(const document_const_iterator& other) { return _index - other._index; }

    bool operator != (document_const_iterator& other) const { return _index != other._index; }
    bool operator == (document_const_iterator& other) const { return _index == other._index; }
private:

    const document* _parent;
    std::size_t _index;

    friend class document;
};

/// Wrapper around document library
/// We need only very minimalistic set of operation to perform on a document,
/// this makes easy to create an abstraction to hide the actual implementation behind.
class document
{
public:

    //typedef document_iterator iterator;
    typedef document_const_iterator const_iterator;

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

    void swap(document& another)
    {
        _internal.swap(another._internal);
    }

    void swap(document&& another)
    {
        _internal.swap(another._internal);
    }

    // this one should be private, but its used by range-based for
    document(const Json::Value& internal) : _internal(internal) { }

    // field access

    bool has_field(const std::string& name) const
    {
        return _internal.isMember(name);
    }

    template<typename T>
    T get(const std::string& field_name) const
    {
        return document(_internal[field_name]).as<T>();
    }

    template<typename T>
    T get_dotted(const std::string& dotted_name) const
    {
        std::size_t dot_pos = dotted_name.find_first_of('.');
        if (dot_pos == std::string::npos)
        {
            return this->get<T>(dotted_name); // not so dotted after all
        }
        else
        {
            std::string before_dot = dotted_name.substr(0, dot_pos);
            std::string after_dot = dotted_name.substr(dot_pos+1);
            return get<document>(before_dot).get_dotted<T>(after_dot);
        }
    }

    // field manipulation
    template<typename T>
    void append(const std::string& field_name, const T& value)
    {
        _internal[field_name] = Json::Value(value); // rely on the existence of compatible constructor
    }

    // conversion to/from sinple values

    template<typename T>
    T as() const;

    template<typename T>
    static
    document from(const T& value)
    {
        return document(Json::Value(value)); // rely on the existence of compatible constructor
    }

    // array

    static
    inline
    document empty_array()
    {
        return Json::Value(Json::arrayValue);
    }

    /// Append to array
    template<typename T>
    void append(const T& v)
    {
        _internal.append(v);
    }

    template<typename T>
    static
    document from_array(const std::vector<T>& array)
    {
        document d = empty_array();
        for( const T& item: array)
        {
            d.append(item); // rely on the existence of compatible constructor
        }
        return d;
    }

    std::size_t size() const
    {
        return _internal.size();
    }

    document operator[](std::size_t idx) const
    {
        return _internal[static_cast<Json::Value::ArrayIndex>(idx)];
    }

    const_iterator insert(const const_iterator& pos, document& element);
    const_iterator erase(const const_iterator& first, const const_iterator& last);
    std::vector<document> to_vector() const;

    // other

    bool is_null() const
    {
        return _internal.isNull();
    }

    std::vector<std::string> field_names() const
    {
        return _internal.getMemberNames();
    }

    bool operator<(const document& other) const;

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

    // iteration

    //iterator begin() { return document_iterator(*this, 0); }
    //iterator end() { return document_iterator(*this, size()); }
    const_iterator begin() const { return document_const_iterator(this, 0); }
    const_iterator end() const { return document_const_iterator(this, size()); }

private:

    document(Json::Value&& internal)
    {
        _internal.swap(internal);
    }

    friend std::ostream& operator << (std::ostream& s, const document& d);

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
inline
void document::append<boost::uuids::uuid>(const std::string& field_name, const boost::uuids::uuid& value)
{
    _internal[field_name] = to_string(value);
}

template<>
inline
void document::append<document>(const std::string& field_name, const document& value)
{
    _internal[field_name] = value._internal;
}

template<>
inline
int document::as() const
{
    return _internal.asInt();
}

template<>
inline
std::string document::as() const
{
    return _internal.asString();
}

template<>
inline
document document::from(const boost::uuids::uuid& uuid)
{
    return document(Json::Value(to_string(uuid)));
}

template<>
inline
void document::append<document>(const document& v)
{
    _internal.append(v._internal);
}

inline document document_const_iterator::operator*()
{
    return (*_parent)[_index];
}

//inline void document_const_iterator::operator=(const document_const_iterator& other)
//{
//    _parent = other._parent;
//    _index = other._index;
//}

} // namespace falcondb

#endif // FALCONDB_DOCUMENT_HPP
