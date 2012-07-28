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
    > document_scalar;

typedef boost::make_recursive_variant< // will your eyes explode?
    document_scalar,
    std::vector<boost::recursive_variant_>,
    std::map<std::string, boost::recursive_variant_>
    >::type document_any;

typedef std::vector<document_any> document_array;
typedef std::map<std::string, document_any> document_map;


/// Wrapper clas providing some extra functionality. Does not own the wrapped object!
class document_wrapper
{
public:

    document_wrapper(const document_any& any) : _any(any) { }

    // converters to variants (non-copying)

    const document_array& as_array() const { return boost::get<document_array>(_any); }
    const document_map& as_map() const { return boost::get<document_map>(_any); }
    const document_scalar& as_scalar() const { return boost::get<document_scalar>(_any); }
    const document_any& as_any() const { return _any; }


    template<typename T>
    std::vector<T> as_array_of() const
    {
        const document_array& input = boost::get<document_array>(this->_any); // may throw
        std::vector<T> result;
        result.reserve(input.size());
        std::copy(input.begin(), input.end(), std::back_inserter(result));
        return result;
    }

    template<typename T>
    std::map<std::string, T> as_map_of() const
    {
        const document_map& input = as_map(); // may throw
        std::map<std::string, T> result;
        std::transform(
            input.begin(),
            input.end(),
            std::inserter(result, result.end()),
            [] (const document_map::value_type& pair)
            {
                return std::make_pair(pair.first, boost::get<T>(boost::get<document_scalar>(pair.second)));
            }
        );
        return result;
    }

    template<typename T>
    T as() const
    {
        return boost::get<T>(this->_any);
    }

    // field access

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

    // i/o

    std::string to_json() const;

    // other

    bool operator < (const document_wrapper& other) const;


protected:

    const document_any& _any; // wrapped object
};

/// Document that actually owns the variant
class document : public document_wrapper
{
public:

    document(const document_any& any) : document_wrapper(_owned), _owned(any) { }
    document(document_any&& any) : document_wrapper(_owned), _owned(any) { }
    document(const document& another) : document_wrapper(_owned), _owned(another._owned) {}
    document(document&& another) : document_wrapper(_owned), _owned(another._owned) { }

    document& operator=(const document& another)
    {
        _owned = another._owned;
        return *this;
    }

    document& operator=(document&& another)
    {
        _owned.swap(another._owned);
        return *this;
    }

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

    static document from_json(const std::string& s);

    document_array& as_array() { return boost::get<document_array>(_owned); }
    document_map& as_map() { return boost::get<document_map>(_owned); }
    document_scalar& as_scalar() { return boost::get<document_scalar>(_owned); }
    document_any& as_any() { return _owned; }

    // ??
    const document_array& as_array() const { return boost::get<document_array>(_any); }
    const document_map& as_map() const { return boost::get<document_map>(_any); }
    const document_scalar& as_scalar() const { return boost::get<document_scalar>(_any); }
    const document_any& as_any() const { return _any; }


private:

    document_any _owned;
};

typedef std::vector<document> document_list;

std::ostream& operator<<(std::ostream& o, const document_wrapper& d);

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

}

#endif // FALCONDB_DOCUMENT_HPP
