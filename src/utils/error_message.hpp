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

#ifndef FALCONDB_ERROR_MESSAGE_HPP
#define FALCONDB_ERROR_MESSAGE_HPP

#include "utils/backtrace_data.hpp"
#include "utils/exception.hpp"
#include "utils/string.hpp"

#include <boost/optional.hpp>

#include <ostream>

namespace falcondb {

/// Use to report operation status. to-bool casting operator allows for easy testing
class error_message
{
    struct content
    {
        std::string m;
        backtrace_data b;
    };

public:

    error_message() = default;

    explicit error_message(const exception& e)
    : _content({e.get_message(), e.get_backtrace()})
    { }

    explicit error_message(const char* m, const backtrace_data& b = backtrace_data())
    : _content({m, b})
    { }

    explicit error_message(const std::string& m, const backtrace_data& b = backtrace_data())
    : _content({m, b})
    { }

    operator bool () const { return !!_content; }

    std::string get_message() const { return _content->m; }
    const backtrace_data& get_backtrace() const { return _content->b; }

private:

    boost::optional<content> _content;
};

inline
std::ostream& operator<<(std::ostream& o, const error_message& e)
{
    if (e) return o << e.get_message();
    else return o << "(no error)";
    return o;
}

}

#endif
