#include "console_frontend/frontend.hpp"

#include <boost/asio.hpp>
#include <boost/thread/tss.hpp>

#include <readline/readline.h>

#include <iostream>
#include <algorithm>

namespace falcondb { namespace console_frontend {

static boost::thread_specific_ptr<frontend> static_instance;

frontend::frontend()
:
    _io_service(nullptr)
{
    _dispatcher.add_command("quit", "Exit", [this](const arg_list& al) { handle_quit(al); });
    _dispatcher.add_command("help", "Display this help", [this](const arg_list& al) { _dispatcher.print_help(); });
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
    static_instance.reset();
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

void frontend::handle_quit(const frontend::arg_list &al)
{
    _io_service->stop();
}

void frontend::static_on_text(char* text)
{
    static_instance->_dispatcher.tokenize_and_execute(text);
}

}}