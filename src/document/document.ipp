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

#ifndef FALCONDB_DOCUMENT_IPP
#define FALCONDB_DOCUMENT_IPP

#include "document/document.hpp"

#include <boost/type_traits.hpp>
#include <boost/utility/enable_if.hpp>

namespace falcondb {

/**/
inline
document::document(const document_list& p)
: _variant(p)
{}

inline
document::document(document_list&& p)
: _variant(p)
{}

inline
document::document(const document_object& p)
: _variant(p)
{}

inline
document::document(document_object&& p)
: _variant(p)
{}

inline
document::document(const document_scalar& p)
: _variant(p)
{
}

inline
document::document(document_scalar&& p)
: _variant(p)
{
}
/**/
inline document::document(document&& d)
: _variant(std::move(d._variant))
{
}

inline document::document(const document& d)
: _variant(d._variant)
{
}

inline document::document(const detail::raw_document_any& v)
: _variant(v)
{
}

inline document::document(detail::raw_document_any&& v)
: _variant(v)
{
}


inline const document_list& document::as_list() const
{
    return boost::get<document_list>(_variant);
}

inline const document_object& document::as_object() const
{
    return boost::get<document_object>(_variant);
}
inline const document_scalar& document::as_scalar() const
{
    return boost::get<document_scalar>(_variant);
}
inline document_list& document::as_list()
{
    return boost::get<document_list>(_variant);
}
inline document_object& document::as_object()
{
    return boost::get<document_object>(_variant);
}
inline document_scalar& document::as_scalar()
{
    return boost::get<document_scalar>(_variant);
}

inline document document::from(const document_scalar& scalar)
{
    return document(scalar);
}

inline document document::from(document_scalar&& scalar)
{
    return document(scalar);
}

inline document document::from(const document_object& o)
{
    return document(o);
}

inline document document::from(document_object&& o)
{
    return document(o);
}

inline document document::from(const document_list& o)
{
    return document(o);
}

inline document document::from(document_list&& o)
{
    return document(o);
}

template<typename T>
document document::from(const std::vector<T>& input)
{
    return from(document_list::from(input));
}

// from any std::map
template<typename T>
document document::from(const std::map<std::string, T>& input)
{
    return from(document_object::from(input));
}

template<typename T>
document document::from(const T& t)
{
    return from(document_scalar::from(t));
}

namespace detail
{
    // type traits
    template<typename T>
    class is_vector : public boost::false_type {};

    template<typename T>
    class is_map : public boost::false_type {};

    template<typename T>
    class is_vector<std::vector<T> > : public boost::true_type {};

    template<typename T>
    class is_map<std::map<std::string, T> > : public boost::true_type {};

    /*
    template<typename T>
    const T& document_as(document& d, boost::enable_if<is_vector<T> >::type* e = nullptr)
    {
        return d.as_list().to_list_of<T>();
    }

    template<typename T>
    const T& document_as(document& d, boost::enable_if<is_mapr<T> >::type* e = nullptr)
    {
        return d.as_object().to_map_of<T>();
    }
    */
}

/*
template<typename T>
const T& document::as() const
{
}
*/

}

#endif
