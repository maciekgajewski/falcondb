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
#include "dbengine/database.hpp"
#include "dbengine/commands.hpp"

#include "utils/exception.hpp"
#include "utils/filesystem.hpp"

#include <algorithm>
#include <memory>

namespace falcondb { namespace dbengine {

engine::engine(const engine_config& config, interfaces::storage_backend& backend)
:
    _config(config),
    _storage_backend(backend)
{
    _processor.register_command("insert", commands::insert);
    _processor.register_command("list", commands::list);
    _processor.register_command("remove", commands::remove);
}

engine::~engine()
{
}

void engine::run()
{
    std::cout << "initializing databases from " << _config.data_dir << std::endl;

    bfs::path data_dir_path(_config.data_dir);

    if (bfs::exists(data_dir_path))
    {
        bfs::directory_iterator it(data_dir_path);
        for( ;it != bfs::directory_iterator(); ++it)
        {
            if (it->status().type() == bfs::directory_file)
            {
                std::cout << "trying " << it->path() << " ... ";
                try
                {
                    rwmutex::scoped_write_lock lock(_databases_mutex);
                    interfaces::database_backend_ptr storage = _storage_backend.open_database(it->path().string());
                    interfaces::database_ptr db = std::make_shared<database>(storage, std::ref(_processor));

                    std::string name = it->path().filename().generic_string();
                    _databases.insert(std::make_pair(name, db));

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
    }
    else
    {
        std::cout << "Creating data directory " << data_dir_path << std::endl;
        bfs::create_directory(data_dir_path);
    }
    _processor.run();
}

std::vector<std::string> engine::get_databases()
{
    std::vector<std::string> result;

    rwmutex::scoped_read_lock lock(_databases_mutex);

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
        return it->second;
    }
}

void engine::create_database(const std::string& db_name)
{
    // does the db already exists?
    {
        rwmutex::scoped_read_lock lock(_databases_mutex);

        auto it = _databases.find(db_name);
        if (it != _databases.end())
        {
            return std::make_shared<database_impl>(it->second, std::ref(_processor));
        }
    }

    bfs::path new_db_path = bfs::path(_config.data_dir) / db_name;

    if (bfs::exists(new_db_path))
    {
        throw exception("File ", new_db_path, " already exists");
    }

    bfs::create_directory(new_db_path);
    interfaces::database_backend_ptr backend = _storage_backend.create_database(new_db_path.generic_string());
    interfaces::database_ptr db = std::make_shared<database>(backend, std::ref(_processor));
    _databases.insert(std::make_pair(db_name, db));
}

void engine::drop_database(const std::string& db_name)
{
    // does the db exists?
    auto it = _databases.find(db_name);
    if (it == _databases.end())
    {
        throw exception("Dastabase ", db_name, " does not exists");
    }
    _databases.erase(it);

    bfs::path db_path = bfs::path(_config.data_dir) / db_name;

    if (bfs::exists(db_path))
    {
        bfs::remove_all(db_path);
    }
}

} }
