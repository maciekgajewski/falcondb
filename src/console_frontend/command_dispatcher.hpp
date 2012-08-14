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

#ifndef FALCONDB_CONSOLE_FRONTEND_COMMAND_DISPATCHER_HPP
#define FALCONDB_CONSOLE_FRONTEND_COMMAND_DISPATCHER_HPP

#include <map>
#include <string>
#include <functional>
#include <vector>

namespace falcondb { namespace console_frontend {

/// Tokenizes and dispatches console parameters
class command_dispatcher
{
public:

    // types

    typedef std::vector<std::string> arg_list;
    typedef std::function<void (const arg_list&)> command_handler;

    // methods

    command_dispatcher();

    /// Adds command to the dispatcher
    void add_command(
        const std::string& command,
        const std::string& synopsis,
        const std::string& description,
        const command_handler& handler);

    /// Sets hanler for unrecognized command
    void set_unrecognized_hanlder(const command_handler& handler);

    /// Tokenizes command-line input, executes command
    void tokenize_and_execute(const std::string& commandline);

    /// Prints formatted help to stdout
    void print_help();


private:

    struct command_description
    {
        std::string synopsis;
        std::string description;
        command_handler handler;
    };

    typedef std::map<std::string, command_description> command_map;
    command_map _commands;

    command_handler _unrecognized_hanlder;
};

}}

#endif
