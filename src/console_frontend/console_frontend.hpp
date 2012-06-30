#ifndef FALCONDB_CONSOLE_FRONTEND_HPP
#define FALCONDB_CONSOLE_FRONTEND_HPP

#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/asio/io_service.hpp>

#include <map>
#include <string>
#include <functional>
#include <vector>

namespace falcondb {

/// Interactive DNB fronted. Reads commands from stdin
class console_frontend
{
public:
    console_frontend();

    /// Starts interactive loop. Returns when user quits
    void execute();

private:

    // reading loop

    static void static_on_text(char *text);
    void on_text(const std::string &text);
    void read_handler(boost::asio::posix::stream_descriptor &stdin_stream);

    boost::asio::io_service* _io_service;

    // commands
    typedef std::vector<std::string> arg_list;
    typedef std::function<void (const arg_list&)> command_handler;
    struct command_description
    {
        std::string description;
        command_handler handler;
    };

    void handle_quit(const arg_list& al);
    void handle_help(const arg_list& al);

    typedef std::map<std::string, command_description> command_map;
    command_map _commands;

    void add_command(const std::string& command, const std::string& description, const command_handler& handler);
};

}

#endif
