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

#ifndef FALCONDB_BACKEND_LEVELDB_DATABASE_HPP
#define FALCONDB_BACKEND_LEVELDB_DATABASE_HPP

#include "interfaces/storage_backend.hpp"

#include <leveldb/db.h>

#include <memory>

namespace falcondb { namespace backend_leveldb {

class database : public interfaces::database_backend
{
public:

    /// Named constructor - opens existing database
    static std::shared_ptr<database> open(const std::string& path);

    /// Named constructor - creates new database
    static std::shared_ptr<database> create(const std::string& path);


    virtual ~database();

    virtual void drop();

    virtual void add(range key, range data);
    virtual void del(range key);
    virtual std::string get(range key);
    virtual void for_each(const key_handler& handler);
private:

    database(leveldb::DB* db);

    /// Throws if status is not ok
    static void throw_if_not_ok(const leveldb::Status& status);

    std::unique_ptr<leveldb::DB> _db;
};

} }

#endif
