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

#ifndef FALCONDB_FRONTEND_MONGO_CONNECTION_HPP
#define FALCONDB_FRONTEND_MONGO_CONNECTION_HPP

#include "message.hpp"

#include "frontend/mongo/engine.hpp"

#include <boost/enable_shared_from_this.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <memory>

namespace falcondb { namespace frontend { namespace mongo {

class connection : public boost::enable_shared_from_this<connection>
{
public:
    typedef boost::shared_ptr<connection> pointer;

    connection(boost::asio::io_service& io_service, collection_engine& engine);

    boost::asio::ip::tcp::socket& socket() { return _socket; }

    void start();

private:

    std::uint32_t reqId();

    void handle_read_header(const boost::system::error_code& e, const message::pointer& msg);
    void handle_read_body(const boost::system::error_code& e, const message::pointer& msg);
    void handle_write_msg(const boost::system::error_code& e);

    void handle_query_msg(const message::pointer& msg);
    void handle_insert_msg(const message::pointer& msg);

    void send_reply(const message::pointer& msg, const bson_object_list &obj_list);
    boost::asio::ip::tcp::socket _socket;

    collection_engine& _engine;
};

}}}

#endif
