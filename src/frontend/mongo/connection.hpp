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

#include "interfaces/engine.hpp"

#include <boost/enable_shared_from_this.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <memory>

namespace falcondb { namespace frontend { namespace mongo {

class connection : public boost::enable_shared_from_this<connection>
{
public:
    typedef boost::shared_ptr<connection> pointer;

    connection(boost::asio::io_service& io_service, interfaces::engine& engine);

    boost::asio::ip::tcp::socket& socket() { return _socket; }

    void start();

private:
    void handle_read_header(const boost::system::error_code& e, const message::pointer& msg);
    void handle_read_body(const boost::system::error_code& e, const message::pointer& msg);
    void handle_write_msg(const boost::system::error_code& e);

    void handle_query_msg(const message::pointer& msg);
    void handle_insert_msg(const message::pointer& msg);

    bson_object_list handle_admin_command(query_message& query_msg);
    bson_object_list handle_db_command(const std::string& db_name, query_message& query_msg);
    void handle_db_query(const message::pointer& msg);

    void post_command(const interfaces::database_ptr &db, const std::string& command, const document_list &param);
    void result_handler(const std::string& operation, const interfaces::error_message& err, const document_list& data);
    void send_reply(const message::pointer& msg, const bson_object_list& obj_list);
    void handle_db_query(const std::string& dbname, const message::pointer& msg, const query_message& query_msg);

    boost::asio::ip::tcp::socket _socket;

    interfaces::engine& _engine;
};

}}}

#endif
