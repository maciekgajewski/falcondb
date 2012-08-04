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

#include "document/detail/variants.hpp"
#include "document/document_list.hpp"
#include "document/document_object.hpp"

namespace falcondb {

/// Provides some extra functinality on top of document_any

// TODO fix it to work!
// looks like this: https://svn.boost.org/trac/boost/ticket/7120
//class document : public detail::raw_document_any
class document
{
public:

    /**/
    document(const document_list& p);
    document(document_list&& p);
    document(const document_object& p);
    document(document_object&& p);
    document(const document_scalar& p);
    document(document_scalar&& p);
    /**/

    document(const detail::raw_document_any& v);
    document(detail::raw_document_any&& v);
    document(document&& d);
    document(const document& d);

    document& operator =(const document& d) = default;

    // converters to variants (non-copying)

    const document_list& as_list() const;
    const document_object& as_object() const;
    const document_scalar& as_scalar() const;
    document_list& as_list();
    document_object& as_object() ;
    document_scalar& as_scalar();

    static document from(const document& doc);
    static document from(document&& doc);
    static document from(const document_scalar& scalar);
    static document from(document_scalar&& scalar);
    static document from(const document_object& obj);
    static document from(document_object&& obj);
    static document from(const document_list& obj);
    static document from(document_list&& obj);

    // from any std::vector
    template<typename T>
    static document from(const std::vector<T>& input);
    // from any std::map
    template<typename T>
    static document from(const std::map<std::string, T>& input);

    // from anything
    static document from(const char* t) { return from(std::string(t)); }

    template<std::size_t S>
    static document from(const char t[S]) { return from(std::string(t)); }

    template<typename T>
    static document from(const T& t);

    // TODO add && versions of from(...)

    // smart - to-anything converter
    template<typename T>
    const T& as() const;

    // i/o

    std::string to_json() const;
    static document from_json(const std::string& s);

    // other

    bool operator < (const document& other) const;
    void swap(document& other) { _variant.swap(other._variant); }

    const detail::raw_document_any& _v() const { return _variant; }

private:

    detail::raw_document_any _variant;

};

std::ostream& operator<<(std::ostream& o, const document& d);

}

#include "document/document.ipp"
#include "document/document_scalar.ipp"
#include "document/document_list.ipp"
#include "document/document_object.ipp"

#endif // FALCONDB_DOCUMENT_HPP
