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

/// The document serializer accepts documents in various forms, including
/// variant types, tuples and containers, breaks them down to fundamental types
/// and passes them to writer for actual writing
template<typename writer_type>
class document_serializer
{
public:

    template<typename T>
    static void write(writer_type& writer, const T& val)
    {
        document_serializer serializer(writer);
        serializer.do_write(val);
    }

private:

    //////////////
    // actual writing

    void do_write(const document_scalar& s)
    {
        boost::apply_visitor(scalar_visitor(this->_writer), s);
    }


    /// writes simple/unknown types (will fail at compile tiem if wrtier does not support it)
    template<typename T>
    void do_write(const T& t)
    {
        _writer.write(t);
    }

    struct scalar_visitor : public boost::static_visitor<>
    {
        scalar_visitor(writer_type& w) : _writer(w) { }

        template<typename T>
        void operator()(const T& t) const
        {
            _writer.write(t);
        }

        writer_type& _writer;
    };

    //////////
    // other

    document_serializer(writer_type& writer)
    : _writer(writer)
    {
        _writer.begin();
    }

    ~document_serializer()
    {
        _writer.end();
    }

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
