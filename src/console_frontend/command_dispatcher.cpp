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
    boost::tokenizer<> tok(commandline);
    boost::tokenizer<>::iterator begin = tok.begin();
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
    }
    else
    {
        it->second.handler(arguments);
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

    // print them out
    std::cout << "Available commands: " << std::endl;
    for(auto i : _commands)
    {
        std::cout << "  " << i.second.synopsis << "  ";
        // padding
        std::cout << std::string(' ', longest_it->second.synopsis.length() - i.second.synopsis.length());
        std::cout << "- " << i.second.description << std::endl;
    }
}

}}
