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

#include "dbengine/command_processor.hpp"

#include "utils/rwmutex.hpp"


namespace falcondb { namespace dbengine {

struct engine_config
{
    std::string data_dir; // main data directory
};

class engine : public interfaces::engine
{
public:
    engine(const engine_config& config, interfaces::storage_backend& backend);
    engine(const engine&) = delete;
    virtual ~engine();

    /// Initializes the database, then spawns the engine worker threads and return
    void run();

    // API
    virtual void create_session(
        const std::string& session_name,
        const create_session_callback& callback);

private:

    void create_database(const std::string& db_name);
    void drop_database(const std::string& db_name);
    interfaces::database_ptr get_database(const std::string& db_name);

    engine_config _config;
    interfaces::storage_backend& _storage_backend;

    // databases
    typedef std::map<std::string, interfaces::database_ptr> database_map;
    database_map _databases;
    rwmutex _databases_mutex;

    // TODO: temporary - internal session mamagement
    typedef std::map<std::string, interfaces::session_ptr> session_map;
    session_map _sessions;
    rwmutex _sessions_mutex;

    friend class session;

    void session_closed(const std::string& session_name);
    void open(const std::string& path, const interfaces::session::open_channel_callback& callback);

    command_processor _processor;
};

} }

#endif
