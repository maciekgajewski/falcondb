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

#include <boost/variant.hpp>
#include <boost/date_time.hpp>

#include <map>
#include <vector>
#include <cstdint>


#ifndef FALCONDB_DYNAMIC_DOCUMENT_HPP
#define FALCONDB_DYNAMIC_DOCUMENT_HPP

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
    >::type document;

typedef std::vector<document> document_array;
typedef std::map<std::string, document> document_map;

inline document document_from(const document_scalar& scalar)
{
    return document(scalar);
}

template<typename T>
document document_from(const std::vector<T>& v)
{
    document_array variant_array;
    variant_array.reserve(v.size());
    std::copy(v.begin(), v.end(), std::back_inserter(variant_array));
    return document(std::move(variant_array));
}
template<>
inline
document document_from<document>(const document_array& v)
{
    return document(v);
}

template<typename T>
document document_from(const std::map<std::string, T>& m)
{
    document_map variant_map;
    std::transform(
        m.begin(),
        m.end(),
        std::inserter(variant_map, variant_map.end()),
        [](const std::pair<std::string, T>& in)
        {
            return std::pair<std::string, document>(in.first, document_from(in.second));
        });
    return document(std::move(variant_map));
}

}

#endif
