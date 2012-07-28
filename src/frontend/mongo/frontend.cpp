/*
FalconDB, a database
Copyright (C) 2012 Kamil Zbrog <kamil.zbrog at gmail dot com>

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

#include "frontend/mongo/frontend.hpp"

#include "utils/exception.hpp"

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/placeholders.hpp>

#include <boost/bind.hpp>

#include <boost/make_shared.hpp>

#include <iostream>
#include <algorithm>
#include <thread>

namespace falcondb { namespace frontend { namespace mongo {

server::server(interfaces::engine& engine)
:
    _engine(engine),
    _io_service(nullptr),
    _acceptor(nullptr)
{
}

void server::start_accept()
{
    connection::pointer conn = boost::make_shared<connection>(*_io_service, _engine);
    _acceptor->async_accept(
        conn->socket(),
        [this, conn](const boost::system::error_code &e)
        {
            this->handle_accept(e, conn);
        });
}

void server::handle_accept(const boost::system::error_code& error, const connection::pointer &conn)
{
    std::cout << "Connection accepted" << std::endl;
    if (!error)
    {
        conn->start();
    }

    start_accept();
}

void server::execute()
{
    // setup asio

    boost::asio::io_service io;

    _io_service = &io;

    using namespace boost::asio::ip;

    tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), 27017));

    _acceptor = &acceptor;

    try
    {
        std::thread worker(
            [this]() {
                start_accept();
                _io_service->run();
            });
        boost::asio::io_service::work w(io);
        io.run();
    }
    catch(const std::exception& e)
    {
        std::cout << "Error in mongo frontend: " << e.what() << std::endl;
    }

    std::cout << "bye bye..." << std::endl;
}

}}}
