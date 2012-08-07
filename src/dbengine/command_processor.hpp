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

#ifndef FALCONDDB_ENGINE_COMMAND_PROCESSOR_HPP
#define FALCONDDB_ENGINE_COMMAND_PROCESSOR_HPP

#include "dbengine/database.hpp"

#include "interfaces/engine.hpp"

#include "utils/rwmutex.hpp"

#include <boost/asio/io_service.hpp>
#include <boost/thread.hpp>

#include <memory>
#include <unordered_map>

namespace falcondb { namespace dbengine {

class command_processor
{
public:
    typedef std::function<void (const document&, const interfaces::result_handler&, database&)> command_handler;


    command_processor();
    //command_
    ~command_processor();

    // starts the command-processing thread
    void run();

    // registers db command
    void register_command(const std::string& command, const command_handler& handler);

    // posts command for execution
    void post(const std::string& command,
        const document& params,
        const interfaces::result_handler& result,
        database& db);

private:

    // commands
    typedef std::unordered_map<std::string, command_handler> command_handler_map;
    command_handler_map _command_handlers;
    rwmutex _command_handlers_mutex;

    // worker thread
    boost::asio::io_service _io_service;
    std::unique_ptr<boost::thread> _thread;
    std::unique_ptr<boost::asio::io_service::work> _work;

    static void handler_wrapper(
        const std::string& command,
        const document& params,
        const interfaces::result_handler& result,
        dbengine::database& db,
        const command_handler& handler);

};

} }

#endif
