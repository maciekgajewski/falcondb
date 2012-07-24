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

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/type_traits.hpp>
#include <boost/utility/enable_if.hpp>

#include <ostream>

namespace falcondb {

/// Wrtier implements basic writing
class json_writer
{
public:

    json_writer(std::ostream& out);

    // document bpoundaries
    void begin() {}
    void end() {}

    /// string - just encode
    void write(const std::string& s) { _out << encode_to_json(s); }

    // ptime - convert to iso string
    void write(const boost::posix_time::ptime& pt)
    {
        write(to_iso_string(pt));
    }

    /// arithmetic type - convert to string
    template<typename T>
    void write(const T& t, typename boost::enable_if<boost::is_arithmetic<T> >::type* dummy = nullptr)
    {
        _out << t;
    }

private:

    /// encodes string to json string
    static std::string encode_to_json(const std::string& s);

    std::ostream& _out;
};

}

#endif
