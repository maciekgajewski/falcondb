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

#ifndef FALCONDB_EXCEPTION_HPP
#define FALCONDB_EXCEPTION_HPP

#include "utils/string.hpp"
#include "utils/backtrace_data.hpp"

#include <stdexcept>

namespace falcondb {

class exception : public std::exception
{
public:

    // simple constructor with pre-formatted message
    explicit exception(const char* what);

    // formatting constructor
    template<typename ...T>
    explicit
    exception(T ...v)
    : _what(build_string(v...)), _backtrace()
    { }

    virtual ~exception() throw();

    virtual const char* what() const throw() { return _what.c_str(); }
    const backtrace_data& get_backtrace() const { return _backtrace; }
    std::string get_message() const { return _what; }

private:

    const std::string _what;
    const backtrace_data _backtrace;
};

} // namespace falcondb

#endif // FALCONDB_EXCEPTION_HPP
