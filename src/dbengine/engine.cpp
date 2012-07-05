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

#include "dbengine/engine.hpp"
#include "dbengine/database_impl.hpp"

#include "utils/exception.hpp"

#include <boost/filesystem.hpp>

#include <algorithm>
#include <memory>

namespace falcondb { namespace dbengine {

engine::engine(const engine_config& config, interfaces::storage_backend& backend)
:
    _config(config),
    _storage_backend(backend)
{
}

engine::~engine()
{
}

void engine::run()
{
    namespace bfs = boost::filesystem3;

    std::cout << "initializing databases from " << _config.data_dir << std::endl;
    bfs::directory_iterator it(_config.data_dir);
    for( ;it != bfs::directory_iterator(); ++it)
    {
        if (it->status().type() == bfs::directory_file)
        {
            std::cout << "trying " << it->path() << " ... ";
            try
            {
                interfaces::database_backend_ptr db = _storage_backend.open_database(it->path().string());
                // TODO ...
                std::cout << "OK";

            }
            catch(const std::exception& e)
            {
                std::cout << "Failed: " << e.what();
            }
            std::cout << std::endl;
        }
    }
    std::cout << _databases.size() << " databases loaded" << std::endl;
    _processor.run();
}

std::vector<std::string> engine::get_databases()
{
    std::vector<std::string> result;
    std::transform(_databases.begin(), _databases.end(),
        std::back_inserter(result), [](const database_map::value_type& p) { return p.first; });

    return result;
}

interfaces::database_ptr engine::get_database(const std::string& db_name)
{
    rwmutex::scoped_read_lock lock(_databases_mutex);

    auto it = _databases.find(db_name);
    if (it == _databases.end())
    {
        throw exception("no such database: ", db_name);
    }
    else
    {
        return std::make_shared<database_impl>(it->second, std::ref(_processor));
    }
}

void engine::create_database(const std::string& db_name)
{
    namespace bfs = boost::filesystem3;
    bfs::path new_db_path = bfs::path(_config.data_dir) / db_name;

    if (bfs::exists(new_db_path))
    {
        throw exception("File ", new_db_path, " already exists");
    }

    bfs::create_directory(new_db_path);
    _storage_backend.create_database(new_db_path.generic_string());
}

void engine::drop_database(const std::string& db_name)
{
    // TODO
    throw exception("drop_database not implemented");
}

} }
