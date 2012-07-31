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

#include "utils/backtrace_data.hpp"

#include <iostream>
#include <sstream>
#include <iterator>

#include <execinfo.h> // glibc-specific backtrace facilities

namespace falcondb {

backtrace_data backtrace_data::create()
{
    void* buf[1024];
    int size = ::backtrace(buf, 1024);

    char** symbols = ::backtrace_symbols(buf, size);

    std::stringstream ss;
    std::vector<std::string> bt;
    for(int i = 0; i < size; ++i)
    {
        bt.push_back(symbols[i]);
    }
    ::free(symbols);

    return std::move(bt);
}

std::ostream& operator<<(std::ostream& o, const backtrace_data& bt)
{
    std::copy(
        bt._bakctrace.begin(), bt._bakctrace.end(),
        std::ostream_iterator<std::string>(std::cout, "\n"));
    return o;
}


} // namespace falcondb
