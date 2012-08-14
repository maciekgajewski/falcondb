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
#include <readline/history.h>

#include <iostream>
#include <algorithm>

namespace falcondb { namespace console_frontend {

static boost::thread_specific_ptr<frontend> static_instance;

frontend::frontend(interfaces::engine& engine)
:
    _engine(engine),
    _io_service(nullptr)
{
    _dispatcher.add_command("quit", "quit", "Exit",
        [this](const arg_list& al) { handle_quit(al); });
    _dispatcher.add_command("help", "help", "Display this help",
        [this](const arg_list& al) { _dispatcher.print_help(); });
    _dispatcher.set_unrecognized_hanlder(
        [this](const arg_list& al) { handle_command(al); });
}

void frontend::execute()
{
    // setup asio

    boost::asio::io_service io_service;

    int stdin_fd = ::dup(STDIN_FILENO);
    boost::asio::posix::stream_descriptor stdin_stream(io_service, stdin_fd);

    // setup readline
    ::rl_callback_handler_install("ifalcon> ", static_on_text);
    ::using_history();

    char* home = ::getenv("HOME");
    boost::optional<std::string> history_file;
    if (home)
    {
        history_file = std::string(home) + "/.falcondb_history";
        read_history(history_file->c_str());
        ::history_set_pos(::history_length-1);
    }


    // read stdin
    stdin_stream.async_read_some(
                    boost::asio::null_buffers(),
                    [&](const boost::system::error_code&, std::size_t){ read_handler(stdin_stream); });

    // enter loop
    _io_service = &io_service;
    static_instance.reset(this);

    io_service.post([this]() { create_session(); });

    try
    {
        io_service.run();
    }
    catch(const std::exception& e)
    {
        std::cout << "Error in consloe frontend: " << e.what() << std::endl;
    }

    // cleanup
    rl_callback_handler_remove();
    static_instance.release();
    _io_service = nullptr;

    if (home)
    {
        write_history(history_file->c_str());
    }

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

void frontend::post_command(const std::string& object_name, const std::string& command, const document& param)
{
    // TODO
}

void frontend::static_on_text(char* text)
{
    static_instance->_dispatcher.tokenize_and_execute(text);
}

void frontend::handle_quit(const frontend::arg_list &al)
{
    _io_service->stop();
}

void frontend::result_handler(const std::string& operation, const error_message& err, const document& data)
{
    if (err)
    {
        std::cout << operation << " error: " << err << std::endl;
        std::cout << err.get_backtrace() << std::endl;
    }
    else
    {
        std::cout << operation << " successfull" << std::endl;
        std::cout << data.to_json() << std::endl;
    }
    _io_service->post([]{ rl_forced_update_display(); });
}

void frontend::handle_command(const arg_list& al)
{
    // input parsing and validation
    if (al.size() < 1)
    {
        std::cout << "Command syntax: COMMAND [TARGET [PARAM]]" << std::endl;
        return;
    }

    std::string command = al[0];
    std::string target = "/";
    document param = document_scalar::null();
    if (al.size() > 1) target = al[1];
    if (al.size() > 2)
    {
        try
        {
            param = document::from_json(al[2]);
        }
        catch(const std::exception& e)
        {
            std::cout << "Error parsig param: " << e.what() << std::endl;
            return;
        }
    }

    // check session
    if (!_session)
    {
        std::cout << "Error: unable to execute command, session not created yet" << std::endl;
    }

    // do we have the object open
    // TODO


}

void frontend::create_session()
{
    _engine.create_session(
        "interactive session",
        [this](const error_message& e, const interfaces::session_ptr& session)
        {
            session_callback(e, session);
        });
}

void frontend::session_callback(const error_message& e, const interfaces::session_ptr& session)
{
    if (e)
    {
        std::cout << "Error creating session: " << e << std::endl;
        _io_service->stop();
    }
    else
    {
        std::cout << "Session created, ready to accept commands" << std::endl;
        _io_service->post([=](){ _session = session; });
    }
}

}}
