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

#ifndef FALCONDB_DOCUMENT_SCALAR_HPP
#define FALCONDB_DOCUMENT_SCALAR_HPP

#include "document/detail/scalar_variant.hpp"

namespace falcondb {

class document;

// TODO fix it to work!
// looks like this: https://svn.boost.org/trac/boost/ticket/7120
//class document_scalar : public detail::raw_document_scalar
class document_scalar
{
public:

    document_scalar(const detail::raw_document_scalar& v) : _variant(v) { }
    document_scalar(detail::raw_document_scalar&& v) : _variant(v) { }
    document_scalar(const document_scalar& other) = default;

    document_scalar(const document& d);
    document_scalar(document&& d);

    /*
    static
    document_scalar from(const char* c) { return from(std::string(c)); }
    */

    template<typename T>
    static
    document_scalar from(const T& v) { return detail::raw_document_scalar(v); }
    template<typename T>
    T& as();
    template<typename T>
    const T& as() const;

    // null handling
    static document_scalar null();
    bool is_null() const;

    bool operator<(const document_scalar& other) const;
    bool operator==(const document_scalar& other) const;

    // i hate it
    const detail::raw_document_scalar& _v() const { return _variant; }

private:

    detail::raw_document_scalar _variant;

};

}

#endif
