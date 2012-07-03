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
};

}}

#endif
