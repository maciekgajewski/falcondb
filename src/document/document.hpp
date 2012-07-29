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

#include "document/null_type.hpp"

#include "utils/exception.hpp"

#include <boost/variant.hpp>
#include <boost/date_time.hpp>
#include <boost/uuid/uuid.hpp>

#include <map>
#include <vector>
#include <cstdint>

namespace falcondb {

// Variant types for storing dynamic documents, created from dynamic data types
typedef boost::variant<
    std::string,
    std::int32_t,
    std::uint32_t,
    std::int64_t,
    std::uint64_t,
    double,
    bool,
    boost::posix_time::ptime,
    boost::uuids::uuid,
    null_type
    > raw_document_scalar;

class document;
class document_object;

typedef boost::variant<
    raw_document_scalar,
    boost::recursive_wrapper< std::vector<document> >,
    boost::recursive_wrapper< document_object >
    > raw_document_any;


typedef std::vector<document> document_list;
typedef std::map<std::string, document> raw_document_map;

/// Provides some extra functinality on top of document_any
class document : public raw_document_any
{
public:

    document(const raw_document_any& p) : raw_document_any(p) {}
    document(raw_document_any&& p) : raw_document_any(p) {}
    document(const document_list& p) : raw_document_any(p) {}
    document(document_list&& p) : raw_document_any(p) {}
    document(const document_object& p) : raw_document_any(p) {}
    document(document_object&& p) : raw_document_any(p) {}

    // converters to variants (non-copying)

    const document_list& as_list() const { return boost::get<document_list>(*this); }
    const document_object& as_object() const { return boost::get<document_object>(*this); }
    const raw_document_scalar& as_scalar() const { return boost::get<raw_document_scalar>(*this); }
    document_list& as_array() { return boost::get<document_list>(*this); }
    document_object& as_object() { return boost::get<document_object>(*this); }
    raw_document_scalar& as_scalar() { return boost::get<raw_document_scalar>(*this); }

    static document from(const raw_document_scalar& scalar) { return raw_document_any(scalar); }
    static document from(raw_document_scalar&& scalar) { return raw_document_any(scalar); }
    static document from(const document_object& obj) { return raw_document_any(obj); }
    static document from(document_object&& obj) { return raw_document_any(obj); }

    // from any std::vector
    template<typename T>
    static document from(const std::vector<T>& input)
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
    // from any std::map
    template<typename T>
    static document from(const std::map<std::string, T>& input);

    /* remove if not needed
    template<typename T>
    const T& as() const
    {
        return boost::get<T>(this->_any);
    }

    template<typename T>
    T& as() const
    {
        return boost::get<T>(this->_any);
    }
    */

    // field access
    /*
    template<typename T>
    T get_field_as(const std::string& field_name) const
    {
        const document_map& map = boost::get<document_map>(this->_any);
        auto it = map.find(field_name);
        if (it != map.end())
        {
            return boost::get<T>(it->second);
        }
        else
        {
            throw exception("No such field '", field_name, "'");
        }
    }

    inline const document_array& get_field_as_array(const std::string& field_name) const
    {
        const document_map& map = boost::get<document_map>(this->_any);
        auto it = map.find(field_name);
        if (it != map.end())
        {
            return boost::get<document_array>(it->second);
        }
        else
        {
            throw exception("No such field '", field_name, "'");
        }
    }

    inline const document_map& get_field_as_map(const std::string& field_name) const
    {
        const document_map& map = boost::get<document_map>(this->_any);
        auto it = map.find(field_name);
        if (it != map.end())
        {
            return boost::get<document_map>(it->second);
        }
        else
        {
            throw exception("No such field '", field_name, "'");
        }
    }
    */

    // i/o

    std::string to_json() const;
    static document from_json(const std::string& s);

    // other

    bool operator < (const document& other) const;


    // convenience converters

    /*
    template<typename T>
    static
    document from_vector(const std::vector<T>& v)
    {
        document_array variant_array;
        variant_array.reserve(v.size());
        std::copy(v.begin(), v.end(), std::back_inserter(variant_array));
        return document(std::move(variant_array));
    }

    template<typename T>
    static
    document from_map(const std::map<std::string, T>& m)
    {
        document_map variant_map;
        std::transform(
            m.begin(),
            m.end(),
            std::inserter(variant_map, variant_map.end()),
            [](const std::pair<std::string, T>& in)
            {
                return std::pair<std::string, document_any>(in.first, document_from(in.second));
            });
        return document(std::move(variant_map));
    }

    inline document_array& get_field_as_array(const std::string& field_name)
    {
        document_map& map = boost::get<document_map>(this->_owned);
        auto it = map.find(field_name);
        if (it != map.end())
        {
            return boost::get<document_array>(it->second);
        }
        else
        {
            throw exception("No such field '", field_name, "'");
        }
    }
    */



};


std::ostream& operator<<(std::ostream& o, const document& d);

/// Provides some extra functionality on top of document_map
class document_object : public raw_document_map
{
public:

    document_object() = default; // wooohoo!
    document_object(const raw_document_map& map) : raw_document_map(map) {}
    document_object(raw_document_map&& map) : raw_document_map(map) {}

    // check for field's existence
    bool has_field(const std::string& field_name) const { return find(field_name) != end(); }

    // return or throw
    const document& get_field(const std::string& field_name) const;
    document& get_field(const std::string& field_name);

    // io
    std::string to_json() const;
    static document_object from_json(const std::string& s);

};

// converts any container to document list
// requirements: exists document::from() for the item held by the container
template<typename iterator_type>
document_list to_document_list(iterator_type begin, iterator_type end)
{
    document_list result;
    result.reserve(std::distance(begin, end));
    std::transform(begin, end, std::back_inserter(result),
        [](const typename iterator_type::reference i)
        {
            return document::from(i);
        });
    return result;
}

// converts any map to document map
template<typename iterator_type>
document_object to_document_object(iterator_type begin, iterator_type end)
{
    document_object result;
    std::transform(begin, end, std::inserter(result, result.end()),
        [](const typename iterator_type::reference i)
        {
            return std::make_pair(i.first, document::from(i.second));
        });
    return result;
}
template<typename T>
document document::from(const std::map<std::string, T>& input)
{
    document_object result;
    std::transform(input.begin(), input.end(), std::inserter(result, result.end()),
        [](const std::pair<std::string, T>& i)
        {
            return std::make_pair(i.first, document::from(i.second));
        });
    return result;
}


/*
template<>
inline
document document::from_vector<document_any>(const document_array& v)
{
    return document(v);
}

template<>
inline
document_array document_wrapper::as_array_of<document_any>() const
{
    return boost::get<document_array>(_any); // may throw
}

template<>
inline
document_map document_wrapper::as_map_of<document_any>() const
{
    return boost::get<document_map>(_any); // may throw
}

template<>
inline
document document_wrapper::get_field_as<document>(const std::string& field_name) const
{
    const document_map& map = boost::get<document_map>(this->_any);
    auto it = map.find(field_name);
    if (it != map.end())
    {
        return document(it->second);
    }
    else
    {
        throw exception("No such field '", field_name, "'");
    }
}
*/
}

#endif // FALCONDB_DOCUMENT_HPP
