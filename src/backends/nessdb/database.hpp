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

#ifndef FALCONDB_BACKEND_NESSDB_DATABASE_HPP
#define FALCONDB_BACKEND_NESSDB_DATABASE_HPP

#include "interfaces/storage_backend.hpp"

extern "C" {
#include <nessdb/db.h>
}

namespace falcondb { namespace backend_nessdb {

class database : public interfaces::database_backend
{
public:

    virtual ~database();

    virtual void drop();

    virtual void add(range key, range data);
    virtual void del(range key);
    virtual std::string get(range key);
    virtual void for_each(const key_value_handler& handler);


private:

    database(const std::string& path);
    friend class backend;

    nessdb* _db;

};

} }

#endif
