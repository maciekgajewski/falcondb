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
    _dispatcher.add_command("create", "create DBNAME", "Create new database",
        [this](const arg_list& al) { handle_create_db(al); });
    _dispatcher.add_command("drop", "drop DBNAME", "Drop existing database",
        [this](const arg_list& al) { handle_drop_db(al); });
    _dispatcher.add_command("insert", "insert DATABASE DOCUMENT", "Upsert document into database",
        [this](const arg_list& al) { handle_insert(al); });
    _dispatcher.add_command("list", "list DATABASE", "Get the entire content of the db",
        [this](const arg_list& al) { handle_list(al); });
    _dispatcher.add_command("remove", "remove DATABASE KEY", "Remove document with _id=KEY from db",
        [this](const arg_list& al) { handle_remove(al); });
    _dispatcher.add_command("showdbs", "showdbs", "List databases",
        [this](const arg_list&) { handle_showdbs(); });
}

void frontend::execute()
{
    // setup asio

    boost::asio::io_service io_service;

    int stdin_fd = ::dup(STDIN_FILENO);
    boost::asio::posix::stream_descriptor stdin_stream(io_service, stdin_fd);

    // setup readline
    rl_callback_handler_install("ifalcon> ", static_on_text);

    char* home = ::getenv("HOME");
    boost::optional<std::string> history_file;
    if (home)
    {
        history_file = std::string(home) + "/.falcondb_history";
        read_history(history_file->c_str());
    }


    // read stdin

    stdin_stream.async_read_some(
                    boost::asio::null_buffers(),
                    [&](const boost::system::error_code&, std::size_t){ read_handler(stdin_stream); });

    // enter loop
    _io_service = &io_service;
    static_instance.reset(this);

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

void frontend::post_command(
    const std::string& db_name,
    const std::string& command,
    const document_list& param)
{
    interfaces::database_ptr db = _engine.get_database(db_name);

    db->post(command, param,
             [this, command](const interfaces::error_message& error, const document_list& data)
             {
                result_handler(command, error, data);
            });
}

void frontend::static_on_text(char* text)
{
    static_instance->_dispatcher.tokenize_and_execute(text);
}

void frontend::handle_quit(const frontend::arg_list &al)
{
    _io_service->stop();
}

void frontend::handle_create_db(const frontend::arg_list& al)
{
    _engine.create_database(require_arg(al, 0));
    std::cout << "ok" << std::endl;
}

void frontend::handle_drop_db(const frontend::arg_list& al)
{
    _engine.drop_database(require_arg(al, 0));
    std::cout << "ok" << std::endl;
}

void frontend::result_handler(const std::string& operation, const interfaces::error_message& err, const document_list& data)
{
    if (err)
    {
        std::cout << operation << " error: " << err << std::endl;
    }
    else
    {
        std::cout << operation << " successfull" << std::endl;
        if (data.empty()) std::cout << "no data returned" << std::endl;
        else
        {
            std::cout << data.size() << " documents returned" << std::endl;
            std::copy(data.begin(), data.end(), std::ostream_iterator<document>(std::cout, "\n"));
            std::cout << std::endl;
        }
    }
    _io_service->post([]{ rl_forced_update_display(); });
}

void frontend::handle_insert(const frontend::arg_list& al)
{
    std::string db_name = require_arg(al, 0);
    document_list doc_list;
    doc_list.push_back(document::from_json(require_arg(al, 1)));
    post_command(db_name, "insert", doc_list);
}

void frontend::handle_list(const frontend::arg_list& al)
{
    std::string db_name = require_arg(al, 0);
    post_command(db_name, "list");
}

void frontend::handle_remove(const frontend::arg_list& al)
{
    std::string db_name = require_arg(al, 0);
    document_list doc_list;
    doc_list.push_back(document::from_json(require_arg(al, 1)));
    post_command(db_name, "remove", doc_list);
}

void frontend::handle_showdbs()
{
    std::cout << "Databases: " << std::endl;
    for(const std::string& db_name : _engine.get_databases())
    {
        std::cout << " * " << db_name << std::endl;
    }
}

}}
