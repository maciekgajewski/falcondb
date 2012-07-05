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

#include "backend_nessdb/database.hpp"

#include <utils/exception.hpp>

#include <nessdb/util.h>

#include <cassert>

namespace falcondb { namespace backend_nessdb {

database::database(const std::string& path)
{
    std::size_t buffer_pool_size = 16*1024*1024;
    _db = ::db_open(buffer_pool_size, path.c_str(), 1);
}

database::~database()
{
    ::db_close(_db);
    _db = nullptr;
}

void database::drop()
{
    // TODO
}

void database::add(range key, range data)
{
    assert(_db);
    struct slice sk, sv;

    sk.data  = const_cast<char*>(key.begin());
    sk.len = key.size();
    sv.data  = const_cast<char*>(data.begin());
    sv.len = data.size();

    if (::db_add(_db, &sk, &sv) == 0)
    {
        throw exception("error adding to ness backend");
    }
}

void database::del(range key)
{
    assert(_db);
    struct slice sk;

    sk.data  = const_cast<char*>(key.begin());
    sk.len = key.size();

    ::db_remove(_db, &sk);
}

std::string database::get(range key)
{
    assert(_db);
    struct slice sk, sv;

    sk.data  = const_cast<char*>(key.begin());
    sk.len = key.size();

    if (::db_get(_db, &sk, &sv) == 0)
    {
        throw exception("error reading from ness backend");
    }

    // copy the data out
    std::string result(sv.data, sv.len);
    ::free(sv.data);
    return result;
}

void database::for_each(const interfaces::database_backend::key_handler& handler)
{
    // undoable with current nessdb
    throw exception("for_each not implemented in nessdb backend");
}

} }
