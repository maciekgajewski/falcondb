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

#include "document/json_writer.hpp"

namespace falcondb {

namespace detail {

struct visitor : public boost::static_visitor<>
{
    visitor(json_writer& writer) : _writer(writer) { }

    template<typename T>
    void operator()(const T& t)
    {
        _writer.write(t);
    }

    json_writer& _writer;
};

}

json_writer::json_writer(std::ostream& out)
: _out(out)
{
}

void json_writer::write(const raw_document_scalar& scalar)
{
    detail::visitor v(*this);
    boost::apply_visitor(v, scalar);
}

std::string json_writer::encode_to_json(const std::string& s)
{
    // TODO add proper escaping
    return std::string("\"") + s + std::string("\"");
}

void json_writer::write(const raw_document_any& doc)
{
    detail::visitor v(*this);
    boost::apply_visitor(v, doc);
}

}

