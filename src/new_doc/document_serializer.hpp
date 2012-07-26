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

#ifndef FALCONDB_DOCUMENT_SERIALIZER_HPP
#define FALCONDB_DOCUMENT_SERIALIZER_HPP

#include "new_doc/dynamic_document.hpp"

#include <boost/utility/enable_if.hpp>

namespace falcondb {

/// Writer requirements:
///
/// writer should have 3 methods:
/// * void writer::wrtie_scalar(const T& v, const std::string& key)
/// * writer::array_wrtier wrtier::write_array(std::size_t, const std::string& key)
/// * writer::map_writer wrtier::write_map(std::size_t, const std::string& key)
///
/// Array wrtier:
/// - the same as top-level wrtier
///
/// Map wrtier:
/// * void writer::wrtie_scalar(const std::string& key, const T& v)
/// * writer::array_wrtier wrtier::write_array(const std::string& key, std::size_t)
/// * writer::map_writer wrtier::write_map(const std::string& key, std::size_t)
///

namespace detail {

// tuple unpacker
template<unsigned I>
struct tuple_unpacker
{
    template<class tuple_type, class serializer_type>
    void operator()(const tuple_type& tuple, serializer_type& s)
    {
        s.do_write(std::get<std::tuple_size<tuple_type>::value - I>(tuple), std::string());
        tuple_unpacker<I-1> next;
        next(tuple, s);
    }
};

template<>
struct tuple_unpacker<1>
{
    template<class tuple_type, class serializer_type>
    void operator()(const tuple_type& tuple, serializer_type& s)
    {
        s.do_write(std::get<std::tuple_size<tuple_type>::value - 1>(tuple), std::string());
    }
};

}


/// The document serializer accepts documents in various forms, including
/// variant types, tuples and containers, breaks them down to fundamental types
/// and passes them to writer for actual writing
template<typename writer_type>
class document_serializer
{
public:

    document_serializer(writer_type& writer)
    : _writer(writer)
    {
    }

    template<typename T>
    static void write(writer_type& writer, const T& val)
    {
        document_serializer serializer(writer);
        serializer.do_write(val, std::string());
    }

// TODO how to make these private?

    //////////////
    // actual writing

    // writes single scalar
    void do_write(const document_scalar& s, const std::string& field_name)
    {
        boost::apply_visitor(scalar_visitor(this->_writer, field_name), s);
    }

    // writes any form of document
    void do_write(const document& d, const std::string& field_name)
    {
        // TODO
    }

    // writes simple array
    template<typename T>
    void do_write(const std::vector<T>& array, const std::string& field_name)
    {
        typename writer_type::array_writer aw = _writer.write_array(array.size(), field_name);
        document_serializer<typename writer_type::array_writer> nested(aw);
        for( const T& i : array )
        {
            nested.do_write(i, std::string());
        }
    }

    // writes simple map
    template<typename T>
    void do_write(const std::map<std::string, T>& map, const std::string& field_name)
    {
        typename writer_type::map_writer mw = _writer.write_map(map.size(), field_name);
        document_serializer<typename writer_type::map_writer> nested(mw);
        for( auto pair : map )
        {
            nested.do_write(pair.second, pair.first);
        }

    }

    // write tuple
    template<typename ...Ts>
    void do_write(const std::tuple<Ts...>& tuple, const std::string& field_name)
    {
        typename writer_type::array_writer aw = _writer.write_array(sizeof...(Ts), field_name);
        document_serializer<typename writer_type::array_writer> nested(aw);
        detail::tuple_unpacker<sizeof...(Ts)> unpacker;
        unpacker(tuple, nested);
    }


    /// writes simple/unknown types (will fail at compile tiem if wrtier does not support it)
    template<typename T>
    void do_write(const T& t, const std::string& field_name)
    {
        _writer.write_scalar(t, field_name);
    }

    struct scalar_visitor : public boost::static_visitor<>
    {
        scalar_visitor(writer_type& w, const std::string& fn)
        : _writer(w), _field_name(fn)  { }

        template<typename T>
        void operator()(const T& t) const
        {
            _writer.write_scalar(t, _field_name);
        }

        writer_type& _writer;
        const std::string& _field_name;
    };

    //////////
    // other

    writer_type& _writer;
};

// helper function
template<typename writer_type, typename document_type>
void write_document(writer_type& writer, const document_type& document)
{
    document_serializer<writer_type>::write(writer, document);
}

}

#endif
