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

#include "document/document_scalar.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <type_traits>

namespace falcondb {

struct scalar_less_visitor : boost::static_visitor<bool>
{
    // the same types - use native conparison
    template<typename T>
    bool operator()(const T& a, const T& b) const
    {
        return a < b;
    }

    // everything is greater than null
    template<typename T>
    typename std::enable_if<!std::is_same<T, null_type>::value, bool>::type
    operator()(const null_type&, const T&) const
    {
        return true;
    }
    template<typename T>
    typename std::enable_if<!std::is_same<T, null_type>::value, bool>::type
    operator()(const T&, const null_type&) const
    {
        return false;
    }

    // different types, both arithmetic
    template<typename T, typename U>
    typename std::enable_if<std::is_arithmetic<T>::value && std::is_arithmetic<U>::value, bool>::type
    operator()(const T& a, const U& b) const
    {
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wsign-compare"
        return a < b;
        #pragma GCC diagnostic pop
    }

    // different types, at least one nont-arithemtic. convert to string
    template<typename T, typename U>
    typename std::enable_if<!std::is_arithmetic<T>::value || !std::is_arithmetic<U>::value, bool>::type
    operator()(const T& a, const U& b) const
    {
        return boost::lexical_cast<std::string>(a) < boost::lexical_cast<std::string>(b);
    }

};

bool document_scalar::operator<(const document_scalar& other) const
{
    return boost::apply_visitor(scalar_less_visitor(), _variant, other._variant);
}


}

