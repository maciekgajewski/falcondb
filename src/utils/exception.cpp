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

#include "exception.hpp"

#include <sstream>

#include <execinfo.h> // glibc-specific backtrace facilities

namespace falcondb {

exception::exception(const char* what)
:
    _what(what),
    _backtrace(get_backtrace())
{
}

exception::~exception() throw()
{
}

std::string exception::get_backtrace()
{
    void* buf[1024];
    int size = ::backtrace(buf, 1024);

    char** symbols = ::backtrace_symbols(buf, size);

    std::stringstream ss;
    for(int i = 0; i < size; ++i)
    {
        ss << symbols[i] << "\n";
    }
    ::free(symbols);
    return ss.str();
}

}
