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

#ifndef FALCONDB_DOCUMENT_SCALAR_IPP
#define FALCONDB_DOCUMENT_SCALAR_IPP

#include "document/document.hpp"

namespace falcondb {

inline document_scalar::document_scalar(const document& d)
: _variant(d.as_scalar()._v())
{
}

inline document_scalar::document_scalar(document&& d)
: _variant(d.as_scalar()._v())
{
}

template<typename T>
T& document_scalar::as()
{
    return boost::get<T>(_variant);
}

template<typename T>
const T& document_scalar::as() const
{
    return boost::get<T>(_variant);
}

inline document_scalar document_scalar::null()
{
    return  detail::raw_document_scalar(null_type());
    //return null_type();
}

inline bool document_scalar::is_null() const
{
    return _variant.type() == typeid(null_type);
}

}

#endif
