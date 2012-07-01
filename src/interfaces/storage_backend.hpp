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

#ifndef STORAGE_BACKEND_HPP
#define STORAGE_BACKEND_HPP

#include "utils/range.hpp"

#include <memory>

namespace falcondb { namespace interfaces {

class database_backend;

class storage_backend
{
public:
    virtual ~storage_backend() {}

    // Opens exisintg database, throws on error
    virtual std::shared_ptr<database_backend> open_database(const std::string& path) = 0;

    // Create database, throws on error
    virtual std::shared_ptr<database_backend> create_database(const std::string& path) = 0;
};

class database_backend
{
public:
    virtual ~database_backend() {}

    virtual void drop() = 0;

    virtual void add(range key, range data) = 0;
    virtual void del(range key) = 0;
    virtual std::string get(range key) = 0;
};

}}

#endif
