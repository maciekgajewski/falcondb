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

#include "console_frontend/command_dispatcher.hpp"

#include <boost/tokenizer.hpp>

#include <readline/history.h>

namespace falcondb { namespace console_frontend {

command_dispatcher::command_dispatcher()
{
}

void command_dispatcher::add_command(
        const std::string& command,
        const std::string& synopsis,
        const std::string& description,
        const command_handler& handler)
{
    _commands.insert(std::make_pair(command, command_description {synopsis, description, handler}));
}

void command_dispatcher::tokenize_and_execute(const std::string& commandline)
{
    // tokenize
    boost::escaped_list_separator<char> sep("\\", " \t", "'\"");
    boost::tokenizer<boost::escaped_list_separator<char>> tok(commandline, sep);
    boost::tokenizer<boost::escaped_list_separator<char>>::iterator begin = tok.begin();
    if (begin == tok.end())
    {
        // empty command line, do nothing
        return;
    }
    std::string command = *(begin++);
    arg_list arguments(begin, tok.end());
    add_history(commandline.c_str());

    // execute
    auto it = _commands.find(command);
    if (it == _commands.end())
    {
        std::cout << "Unknown command: " << command << std::endl;
        std::cout << "Type 'help' for list of available commands" << std::endl;
    }
    else
    {
        try
        {
            it->second.handler(arguments);
        }
        catch(const std::exception& e)
        {
            std::cout << std::endl;
            std::cout << "Error executing " << commandline << " : " << e.what() << std::endl;
            std::cout << "quick help: " << it->second.synopsis << " - " << it->second.description << std::endl;
        }
    }
}

void command_dispatcher::print_help()
{
    // find the longest synopsis
    auto longest_it = std::max_element(
                _commands.begin(),
                _commands.end(),
                [] (const command_map::value_type& a, const command_map::value_type& b)
                    { return a.second.synopsis.length() < b.second.synopsis.length(); });

    std::size_t longest_synopis = longest_it->second.synopsis.length();

    // print them out
    std::cout << "Available commands: " << std::endl;
    for(auto i : _commands)
    {
        std::cout << "  " << i.second.synopsis << "  ";
        // padding
        std::cout << std::string(longest_synopis - i.second.synopsis.length(), ' ');
        std::cout << "- " << i.second.description << std::endl;
    }
}

}}
