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

#ifndef FALCONDB_JSON_DOCUMENT_WRITER_HPP
#define FALCONDB_JSON_DOCUMENT_WRITER_HPP

#include "document/document.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/type_traits.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <ostream>

namespace falcondb {

namespace detail {
    class json_array_writer;
    class json_map_writer;
}

/// Wrtier implements basic writing
class json_writer
{
public:

    json_writer(std::ostream& out);

    /// string - just encode
    void write(const std::string& s) { _out << encode_to_json(s); }

    void write(const char* s) { _out << encode_to_json(s); }
    template<unsigned S>
    void write(const char s[S]) { _out << encode_to_json(s); }

    // ptime - convert to iso string
    void write(const boost::posix_time::ptime& pt)
    {
        _out << encode_to_json(to_iso_string(pt));
    }

    // uuid as string
    void write(const boost::uuids::uuid& uuid)
    {
        _out << encode_to_json(to_string(uuid));
    }

    // null
    void write(const null_type&)
    {
        _out << "null";
    }

    // bool
    void write(bool b)
    {
        if (b) _out << "true";
        else _out << "false";
    }

    /// arithmetic type - convert to string
    template<typename T>
    void write(
        const T& t,
        typename boost::enable_if<boost::is_arithmetic<T> >::type* dummy = nullptr)
    {
        _out << t;
    }

    /// vector
    template<typename T>
    void write(const std::vector<T>& v)
    {
        std::string sep;
        _out << "[";
        for(const T& i : v)
        {
            _out << sep;
            this->write(i);
            sep = ",";
        }
        _out << "]";
    }

    // map
    template<typename T>
    void write(const std::map<std::string, T>& m)
    {
        std::string sep;
        _out << "{";
        for(const std::pair<std::string, T>& p : m)
        {
            _out << sep;
            _out << encode_to_json(p.first) << ":";
            this->write(p.second);
            sep = ",";
        }
        _out << "}";
    }

    /// variant scalar
    void write(const document_scalar& scalar);

    // variant everything
    void write(const document_any& doc);

private:

    /// encodes string to json string
    static std::string encode_to_json(const std::string& s);

    /// Validates jason map field name. Throws if there is a problem
    static void validate_field_name(const std::string& fn);

    std::ostream& _out;
};

}

#endif
