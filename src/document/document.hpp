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

#include <boost/variant.hpp>
#include <boost/date_time.hpp>

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
    boost::posix_time::ptime> document_scalar;

typedef boost::make_recursive_variant< // will your eyes explode?
    document_scalar,
    std::vector<boost::recursive_variant_>,
    std::map<std::string, boost::recursive_variant_>
    >::type document_any;

typedef std::vector<document_any> document_array;
typedef std::map<std::string, document_any> document_map;


///////////////
// various document-related functions





class document
{
public:

    document(const document_any& any) : any(any) { }
    document(document_any&& any) : any(any) { }

    document_any any;

    // convenience converters

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

    template<typename T>
    std::vector<T> as_array_of() const
    {
        const document_array& input = boost::get<document_array>(this->any); // may throw
        std::vector<T> result;
        result.reserve(input.size());
        std::copy(input.begin(), input.end(), std::back_inserter(result));
        return result;
    }

    template<typename T>
    std::map<std::string, T> as_map_of() const
    {
        const document_map& input = boost::get<document_map>(this->any); // may throw
        std::map<std::string, T> result;
        std::copy(input.begin(), input.end(), std::inserter(result, result.end()));
        return result;
    }

    template<typename T>
    T as() const
    {
        return boost::get<T>(this->any);
    }

    // field access

    template<typename T>
    T get_field_as(const std::string& field_name) const
    {
        const document_map& map = boost::get<document_map>(this->any);
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

    // i/o

    std::string to_json(const document& d) const;
    static document from_json(const std::string& s);


private:

};

typedef std::vector<document> document_list;

std::ostream& operator<<(std::ostream& o, const document& d);

template<>
inline
document document::from_vector<document_any>(const document_array& v)
{
    return document(v);
}

template<>
inline
document_array document::as_array_of<document_any>() const
{
    return boost::get<document_array>(any); // may throw
}

template<>
inline
document_map document::as_map_of<document_any>() const
{
    return boost::get<document_map>(any); // may throw
}

template<>
inline
document document::get_field_as<document>(const std::string& field_name) const
{
    const document_map& map = boost::get<document_map>(this->any);
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

}

#endif // FALCONDB_DOCUMENT_HPP
