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
    void write_scalar(const std::string& s, const std::string&) { _out << encode_to_json(s); }

    // ptime - convert to iso string
    void write_scalar(const boost::posix_time::ptime& pt, const std::string&)
    {
        write_scalar(to_iso_string(pt), std::string());
    }

    // bool
    void write_scalar(bool b, const std::string&)
    {
        if (b) _out << "true";
        else _out << "false";
    }

    /// arithmetic type - convert to string
    template<typename T>
    void write_scalar(
        const T& t,
        const std::string&,
        typename boost::enable_if<boost::is_arithmetic<T> >::type* dummy = nullptr)
    {
        _out << t;
    }

    friend class detail::json_array_writer;
    friend class detail::json_map_writer;
    typedef detail::json_array_writer array_writer;
    typedef detail::json_map_writer map_writer;

    array_writer write_array(std::size_t /*size*/, const std::string&);
    map_writer write_map(std::size_t /*size*/, const std::string&);

private:

    /// encodes string to json string
    static std::string encode_to_json(const std::string& s);

    std::ostream& _out;
};

namespace detail {

class json_array_writer
{
public:

    typedef json_writer::map_writer map_writer;
    typedef json_writer::array_writer array_writer;

    json_array_writer(json_writer& parent)
    : _parent(parent)
    {
        _parent._out << "[ ";
    }

    ~json_array_writer()
    {
        _parent._out << " ]";
    }

    template<typename T>
    void write_scalar(const T& t, const std::string&)
    {
        _parent._out << _sep;
        _parent.write_scalar(t, std::string());
        _sep = " , ";
    }

    map_writer write_map(std::size_t size, const std::string& field_name);
    array_writer write_array(std::size_t size, const std::string& field_name);

private:

    json_writer& _parent;
    std::string _sep;
};

class json_map_writer
{
public:

    typedef json_writer::map_writer map_writer;
    typedef json_writer::array_writer array_writer;

    json_map_writer(json_writer& parent)
    : _parent(parent)
    {
        _parent._out << "{ ";
    }

    ~json_map_writer()
    {
        _parent._out << " }";
    }

    template<typename T>
    void write_scalar(const T& t, const std::string& field_name)
    {
        _parent._out << _sep << field_name << ": ";
        _parent.write_scalar(t, std::string());
        _sep = " , ";
    }

    map_writer write_map(std::size_t size, const std::string& field_name);
    array_writer write_array(std::size_t size, const std::string& field_name);

private:

    json_writer& _parent;
    std::string _sep;
};

} // detail

}

#endif
