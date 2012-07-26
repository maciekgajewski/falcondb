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

namespace falcondb {

/// Writer requirements:
///
/// Top level writer should have 3 methods:
/// * void writer::wrtie_scalar(const T& v)
/// * writer::array_wrtier wrtier::write_array(std::size_t)
/// * writer::map_writer wrtier::write_map(std::size_t)
///
/// Array wrtier:
/// - the same as top-level wrtier
///
/// Map wrtier:
/// * void writer::wrtie_scalar(const std::string& key, const T& v)
/// * writer::array_wrtier wrtier::write_array(const std::string& key, std::size_t)
/// * writer::map_writer wrtier::write_map(const std::string& key, std::size_t)
///


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
        serializer.do_write(val);
    }

// TODO how to make these private?

    //////////////
    // actual writing

    // writes single scalar
    void do_write(const document_scalar& s)
    {
        boost::apply_visitor(scalar_visitor(this->_writer), s);
    }

    // writes any form of document
    void do_write(const document& d)
    {
        // TODO
    }

    // writes simple array
    template<typename T>
    void do_write(const std::vector<T>& array)
    {
        typename writer_type::array_writer aw = _writer.write_array(array.size());
        document_serializer<typename writer_type::array_writer> nested(aw);
        for( const T& i : array )
        {
            nested.do_write(i);
        }
    }

    /// writes simple/unknown types (will fail at compile tiem if wrtier does not support it)
    template<typename T>
    void do_write(const T& t)
    {
        _writer.write_scalar(t);
    }

    struct scalar_visitor : public boost::static_visitor<>
    {
        scalar_visitor(writer_type& w) : _writer(w) { }

        template<typename T>
        void operator()(const T& t) const
        {
            _writer.write_scalar(t);
        }

        writer_type& _writer;
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
