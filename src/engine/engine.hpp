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

#ifndef FALCONDB_ENGINE_ENGINE_HPP
#define FALCONDB_ENGINE_ENGINE_HPP

#include "interfaces/engine.hpp"
#include "interfaces/storage_backend.hpp"

namespace falcondb { namespace engine {

struct engine_config
{
    std::string data_dir; // main data directory
};

class engine_impl : public interfaces::engine
{
public:
    engine_impl(const engine_config& config, interfaces::storage_backend& backend);

    /// Initializes the database, then spawns the engine worker threads and return
    void run();

    // API

    virtual std::vector<std::string> get_databases();
    virtual std::shared_ptr<interfaces::database> get_database(const std::string& db_name);

private:

    engine_config _config;
    interfaces::storage_backend& _storage_backend;
};

} }

#endif
