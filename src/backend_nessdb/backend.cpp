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

#include "backend_nessdb/backend.hpp"
#include "backend_nessdb/database.hpp"

#include "utils/exception.hpp"

#include <boost/filesystem.hpp>

namespace falcondb { namespace backend_nessdb {

backend::backend()
{
}

backend::~backend()
{
}

std::shared_ptr<interfaces::database_backend> backend::open_database(const std::string& path)
{
    // ness dosn't do any checking, lets do some chedcking by ourselves
    if (boost::filesystem3::is_directory(path))
    {
        return std::shared_ptr<interfaces::database_backend>(new database(path));
    }
    else
    {
        throw exception(path, " is not a directory");
    }
}

std::shared_ptr<interfaces::database_backend> backend::create_database(const std::string& path)
{
    if (!boost::filesystem3::exists(path))
    {
        return std::shared_ptr<interfaces::database_backend>(new database(path));
    }
    else
    {
        throw exception(path, " already exists");
    }
}

} }
