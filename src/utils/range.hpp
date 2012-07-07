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

#ifndef FALCONDB_UTILS_RANGE_HPP
#define FALCONDB_UTILS_RANGE_HPP

#include <string>

namespace falcondb {

// utils hould be top-leve, no need to nested namespace for general-pourpose classes

/// Describes, but dosn't own, a binary data range
class range
{
public:
    typedef char value_type;

    range(const std::string& s)
    : _begin(nullptr), _end(nullptr)
    {
        if (!s.empty())
        {
            _begin = reinterpret_cast<const value_type*>(&s[0]);
            _end = _begin + s.length();
        }
    }

    range(const value_type* b, std::size_t s)
    {
        _begin = b;
        _end = b + s;
    }

    const value_type* begin() const { return _begin; }
    const value_type* end() const { return _end; }
    std::size_t size() const { return _end - _begin; }
    bool empty() const { return _begin == _end; }

    std::string to_string() const { return std::string(begin(), size()); }

private:

    const value_type* _begin;
    const value_type* _end;
};



}

#endif
