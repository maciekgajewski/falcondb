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

    void handle_quit(const arg_list& al);
    void handle_create_db(const arg_list& al);
    void handle_drop_db(const arg_list& al);
};

}}

#endif
