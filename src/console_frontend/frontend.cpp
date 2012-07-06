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

#include "console_frontend/frontend.hpp"

#include "utils/exception.hpp"

#include <boost/asio.hpp>
#include <boost/thread/tss.hpp>

#include <readline/readline.h>

#include <iostream>
#include <algorithm>

namespace falcondb { namespace console_frontend {

static boost::thread_specific_ptr<frontend> static_instance;

frontend::frontend(interfaces::engine& engine)
:
    _engine(engine),
    _io_service(nullptr)
{
    _dispatcher.add_command("quit", "quit", "Exit", [this](const arg_list& al) { handle_quit(al); });
    _dispatcher.add_command("help", "help", "Display this help", [this](const arg_list& al) { _dispatcher.print_help(); });
    _dispatcher.add_command("create", "create DBNAME", "Create new database", [this](const arg_list& al) { handle_create_db(al); });
    _dispatcher.add_command("drop", "drop DBNAME", "Drop existing database", [this](const arg_list& al) { handle_drop_db(al); });
}

void frontend::execute()
{
    // setup asio

    boost::asio::io_service io_service;

    int stdin_fd = ::dup(STDIN_FILENO);
    boost::asio::posix::stream_descriptor stdin_stream(io_service, stdin_fd);

    // setup readline
    rl_callback_handler_install("ifalcon> ", static_on_text);

    // read stdin

    stdin_stream.async_read_some(
                    boost::asio::null_buffers(),
                    [&](const boost::system::error_code&, std::size_t){ read_handler(stdin_stream); });

    _io_service = &io_service;
    static_instance.reset(this);
    io_service.run();

    // cleanup
    rl_callback_handler_remove();
    static_instance.release();
    _io_service = nullptr;

    std::cout << "bye bye..." << std::endl;
}

void frontend::read_handler(boost::asio::posix::stream_descriptor& stdin_stream)
{
    rl_callback_read_char();
    stdin_stream.async_read_some(
                    boost::asio::null_buffers(),
                [&](const boost::system::error_code&, std::size_t){ read_handler(stdin_stream); });
}

const std::string& frontend::require_arg(const command_dispatcher::arg_list& al, std::size_t idx)
{
    if (al.size() <= idx)
    {
        throw exception("missing command argument");
    }
    return al[idx];
}

void frontend::handle_quit(const frontend::arg_list &al)
{
    _io_service->stop();
}

void frontend::handle_create_db(const frontend::arg_list& al)
{
    _engine.create_database(require_arg(al, 0));
}

void frontend::handle_drop_db(const frontend::arg_list& al)
{
    _engine.drop_database(require_arg(al, 0));
}

void frontend::static_on_text(char* text)
{
    static_instance->_dispatcher.tokenize_and_execute(text);
}

}}
