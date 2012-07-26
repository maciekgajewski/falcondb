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

#include "new_doc/json_document_writer.hpp"

namespace falcondb {

json_writer::json_writer(std::ostream& out)
: _out(out)
{
}

std::string json_writer::encode_to_json(const std::string& s)
{
    // TODO add proper escaping
    return std::string("\"") + s + std::string("\"");
}

json_writer::array_writer json_writer::write_array(std::size_t /*size*/, const std::string&)
{
    return array_writer(*this);
}

json_writer::map_writer json_writer::write_map(std::size_t /*size*/, const std::string&)
{
    return map_writer(*this);
}

detail::json_array_writer::map_writer detail::json_array_writer::write_map(std::size_t size, const std::string& field_name)
{
    _parent._out << field_name << ": ";
    _sep = " , ";
    return _parent.write_map(size, field_name);
}

detail::json_array_writer::array_writer detail::json_array_writer::write_array(std::size_t size, const std::string& field_name)
{
    _parent._out << field_name << ": ";
    _sep = " , ";
    return _parent.write_array(size, field_name);
}

detail::json_map_writer::map_writer detail::json_map_writer::write_map(std::size_t size, const std::string& field_name)
{
    _parent._out << _sep << field_name << ": ";
    _sep = " , ";
    return _parent.write_map(size, field_name);
}

detail::json_map_writer::array_writer detail::json_map_writer::write_array(std::size_t size, const std::string& field_name)
{
    _parent._out << _sep << field_name << ": ";
    _sep = " , ";
    return _parent.write_array(size, field_name);
}

}

