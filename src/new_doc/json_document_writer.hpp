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

    /// string - just encode
    void write_scalar(const std::string& s) { _out << encode_to_json(s); }

    // ptime - convert to iso string
    void write_scalar(const boost::posix_time::ptime& pt)
    {
        write_scalar(to_iso_string(pt));
    }

    /// arithmetic type - convert to string
    template<typename T>
    void write_scalar(const T& t, typename boost::enable_if<boost::is_arithmetic<T> >::type* dummy = nullptr)
    {
        _out << t;
    }

    // array wrtier
    class array_writer
    {
    public:

        array_writer(json_writer& parent)
        : _parent(parent)
        {
            _parent._out << "[ ";
        }

        ~array_writer()
        {
            _parent._out << " ]";
        }

        template<typename T>
        void write_scalar(const T& t)
        {
            _parent._out << _sep;
            _parent.write_scalar(t);
            _sep = " , ";
        }
    private:
        json_writer& _parent;
        std::string _sep;
    };
    friend class array_writer;

    array_writer write_array(std::size_t /*size*/)
    {
        return array_writer(*this);
    }

private:

    /// encodes string to json string
    static std::string encode_to_json(const std::string& s);

    std::ostream& _out;
};

}

#endif
