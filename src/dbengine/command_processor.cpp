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

#include "dbengine/command_processor.hpp"

#include "utils/exception.hpp"

namespace falcondb { namespace dbengine {

command_processor::command_processor()
{
}

command_processor::~command_processor()
{
    _work.reset();
    if (_thread) _thread->join();
}

void command_processor::run()
{
    // start worker thread
    _work.reset(new boost::asio::io_service::work(_io_service));
    _thread.reset(new boost::thread([this] { _io_service.run(); }));
}

void command_processor::register_command(const std::string& command, const interfaces::command_handler& handler)
{
    rwmutex::scoped_write_lock lock(_command_handlers_mutex);

    auto r = _command_handlers.insert(std::make_pair(command, handler));
    if (!r.second)
    {
        throw exception("command ", command, " already registered");
    }
}

void command_processor::post(
    const std::string& command,
    const document& params,
    const interfaces::result_handler& result,
    const interfaces::database_backend_ptr& storage)
{
    rwmutex::scoped_read_lock lock(_command_handlers_mutex);

    auto it = _command_handlers.find(command);
    if (it == _command_handlers.end())
    {
        throw exception("no such command: ", command);
    }

    const interfaces::command_handler& handler = it->second;
    _io_service.post([=](){ handler_wrapper(command, params, result, storage, handler); });
}

void command_processor::handler_wrapper(const std::string& command,
    const document& params,
    const interfaces::result_handler& result,
    const interfaces::database_backend_ptr& storage,
    const interfaces::command_handler& handler)
{
    try
    {
        handler(params, result, storage);
    }
    catch(const std::exception& e)
    {
        result(std::string(e.what()), document_list());
    }
}

} }
