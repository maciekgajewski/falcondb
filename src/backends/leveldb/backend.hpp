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

#ifndef FALCONDB_BACKEND_LEVELDB_BACKEND_HPP
#define FALCONDB_BACKEND_LEVELDB_BACKEND_HPP

#include "interfaces/storage_backend.hpp"

namespace falcondb { namespace backend_leveldb {

class backend : public interfaces::storage_backend
{
public:
    backend();
    virtual ~backend();

    virtual std::shared_ptr<interfaces::database_backend> open_database(const std::string& path);
    virtual std::shared_ptr<interfaces::database_backend> create_database(const std::string& path);
};

} }

#endif
