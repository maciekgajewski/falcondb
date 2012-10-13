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

#ifndef FALCONDB_FRONTEND_MONGO_HPP
#define FALCONDB_FRONTEND_MONGO_HPP

#include "interfaces/engine.hpp"
#include "frontends/mongo/connection.hpp"

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio.hpp>

namespace falcondb { namespace frontend { namespace mongo {

class server
{
public:
    server(interfaces::engine& engine);

    /// Starts interactive loop. Returns when user quits
    void execute();

private:

    void start_accept();
    void handle_accept(const boost::system::error_code& error, const connection::pointer &conn);

    // data
    interfaces::engine& _engine;

    boost::asio::io_service* _io_service;
    boost::asio::ip::tcp::acceptor* _acceptor;
};

}}}

#endif
