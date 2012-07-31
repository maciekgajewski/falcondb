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

#ifndef FALCONDB_DETAIL_DOCUMENT_SCALAR_VARIANT_HPP
#define FALCONDB_DETAIL_DOCUMENT_SCALAR_VARIANT_HPP


#include "document/null_type.hpp"

#include <boost/variant.hpp>
#include <boost/date_time.hpp>
#include <boost/uuid/uuid.hpp>

#include <cstdint>

namespace falcondb { namespace detail {

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

}}

#endif
