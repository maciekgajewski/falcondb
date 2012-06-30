#include "console_frontend/console_frontend.hpp"

#include <boost/asio.hpp>
#include <boost/tokenizer.hpp>

#include <readline/readline.h>
#include <readline/history.h>

#include <iostream>
#include <algorithm>

namespace falcondb {

static __thread console_frontend* static_instace = nullptr;

console_frontend::console_frontend()
:
    _io_service(nullptr)
{
    add_command("quit", "Exit", [this](const arg_list& al) { handle_quit(al); });
    add_command("help", "Display this help", [this](const arg_list& al) { handle_help(al); });
}

void console_frontend::execute()
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
    static_instace = this;
    io_service.run();

    // cleanup
    rl_callback_handler_remove();
    static_instace = nullptr;
    _io_service = nullptr;

    std::cout << "bye bye..." << std::endl;
}

void console_frontend::read_handler(boost::asio::posix::stream_descriptor& stdin_stream)
{
    rl_callback_read_char();
    stdin_stream.async_read_some(
                    boost::asio::null_buffers(),
                [&](const boost::system::error_code&, std::size_t){ read_handler(stdin_stream); });
}

void console_frontend::handle_quit(const console_frontend::arg_list &al)
{
    _io_service->stop();
}

void console_frontend::handle_help(const console_frontend::arg_list &al)
{
    // find the longest command
    auto longest_it = std::max_element(
                _commands.begin(),
                _commands.end(),
                [] (const command_map::value_type& a, const command_map::value_type& b)
                    { return a.first.length() < b.first.length(); });

    // print them out
    std::cout << "Available commands: " << std::endl;
    for(auto i : _commands)
    {
        std::cout << "  " << i.first << "  ";
        // padding
        std::cout << std::string(' ', longest_it->first.length() - i.first.length());
        std::cout << "- " << i.second.description << std::endl;
    }
}

void console_frontend::static_on_text(char* text)
{
    static_instace->on_text(text);
}

void console_frontend::on_text(const std::string& text)
{
    // tokenize
    boost::tokenizer<> tok(text);
    boost::tokenizer<>::iterator begin = tok.begin();
    if (begin == tok.end())
    {
        // empty command line, do nothing
        return;
    }
    std::string command = *(begin++);
    arg_list arguments(begin, tok.end());
    add_history(text.c_str());

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

void console_frontend::add_command(const std::string& command, const std::string& description, const console_frontend::command_handler& handler)
{
    _commands.insert(std::make_pair(command, command_description {description, handler}));
}

} // namespace falcondb
