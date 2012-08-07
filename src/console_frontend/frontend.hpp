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

#ifndef FALCONDB_CONSOLE_FRONTEND_HPP
#define FALCONDB_CONSOLE_FRONTEND_HPP

#include "console_frontend/command_dispatcher.hpp"

#include "interfaces/engine.hpp"

#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/asio/io_service.hpp>

namespace falcondb { namespace console_frontend {

/// Interactive DNB fronted. Reads commands from stdin
class frontend
{
public:
    frontend(interfaces::engine& engine);

    /// Starts interactive loop. Returns when user quits
    void execute();

private:

    // data
    interfaces::engine& _engine;

    // reading loop

    static void static_on_text(char *text);
    void read_handler(boost::asio::posix::stream_descriptor& stdin_stream);

    boost::asio::io_service* _io_service;
    command_dispatcher _dispatcher;

    // commands

    typedef command_dispatcher::arg_list arg_list;
    /// returns arg at position idx, throws exception if no such argument
    static const std::string& require_arg(const command_dispatcher::arg_list& al, std::size_t idx);

    void post_command(const std::string& db_name, const std::string& command, const document& param = document_list());

    void result_handler(const std::string& operation,
        const error_message& err,
        const document_list& data);

    void handle_quit(const arg_list& al);
    void handle_create_db(const arg_list& al);
    void handle_drop_db(const arg_list& al);
    void handle_insert(const arg_list& al);
    void handle_list(const arg_list& al);
    void handle_listindexes(const arg_list& al);
    void handle_remove(const arg_list& al);
    void handle_showdbs();
    void handle_dump(const arg_list& al);
};

}}

#endif
