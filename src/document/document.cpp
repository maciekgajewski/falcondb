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

#include "document/document.hpp"
#include "document/json_writer.hpp"
#include "document/json_parser.hpp"

#include <sstream>

namespace falcondb {

std::string document::to_json() const
{
    std::ostringstream ss;
    json_writer w(ss);
    w.write(*this);
    return ss.str();
}

document document::from_json(const std::string& s)
{
    return json_parser::parse_doc(s);
}

std::string document_object::to_json() const
{
    return document::from(*this).to_json();
}

document_object document_object::from_json(const std::string& s)
{
    return document::from_json(s).as_object();
}

std::ostream& operator<<(std::ostream& o, const document& d)
{
    json_writer w(o);
    w.write(d);
    return o;
}


bool document::operator < (const document& other) const
{
    return false; // TODO
}

const document& document_object::get_field(const std::string& field_name) const
{
    auto it = find(field_name);
    if (it == end())
        throw exception("Field ", field_name, " not set");
    else
        return it->second;
}

document& document_object::get_field(const std::string& field_name)
{
    auto it = find(field_name);
    if (it == end())
        throw exception("Field ", field_name, " not set");
    else
        return it->second;
}


} // namespace
