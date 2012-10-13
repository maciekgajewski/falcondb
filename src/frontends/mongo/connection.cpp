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

#include "connection.hpp"

#include "message.hpp"

#include "bson/bsonobjbuilder.hpp"
#include "bson/bsonobj.ipp"
#include "bson/bsonelement.ipp"
#include "bson/json.hpp"

#include "utils/log.hpp"

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <boost/assign/list_of.hpp>

#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/placeholders.hpp>

#include <boost/bind.hpp>

namespace falcondb { namespace frontend { namespace mongo {

connection::connection(boost::asio::io_service& io_service)
: _socket(io_service)
{
}

void connection::start(const interfaces::session_ptr &session)
{
    assert(session);

    _session = session;
    start();
}

void connection::start()
{
    logging::info("New connection started ", _socket.remote_endpoint().address().to_string());
    logging::debug("Reading header: ", message::header_length);

    message::pointer msg = std::make_shared<message>();

    boost::asio::async_read(
        _socket,
        boost::asio::buffer(const_cast<char*>(msg->_header.data()), message::header_length),
        boost::bind(&connection::handle_read_header, shared_from_this(), boost::asio::placeholders::error(), msg));
}

void connection::handle_read_header(const boost::system::error_code& e, const message::pointer& msg)
{
    if (e) {
        logging::error("Error reading header: ", e);
    } else {
        const message::header &header = msg->_header;
        std::string &body = msg->_body;

        logging::debug("Header read: ",header);

        body.resize(header._msglen - sizeof(header));

        logging::debug("Reading body: ", body.size());

        boost::asio::async_read(
            _socket,
            boost::asio::buffer(const_cast<char*>(body.data()), body.size()),
            boost::bind(&connection::handle_read_body, shared_from_this(), boost::asio::placeholders::error(), msg));
    }
}

void connection::handle_read_body(const boost::system::error_code& e, const message::pointer& msg)
{
    if (e) {
        logging::error("Error reading body: ", e);
    } else {
        switch (msg->_header._opCode)
        {
            case message::op_code::QUERY:
                handle_query_msg(msg);
                break;
            case message::op_code::INSERT:
                handle_insert_msg(msg);
                break;
        }

        start();
    }
}

void connection::handle_query_msg(const message::pointer& msg)
{
    assert(msg);
    query_message query_msg(*msg);


    std::string query;
    bson_object_list params;
    while (query_msg.has_more()) {
        params.push_back(query_msg.next_obj());
        query += params.back().toString();
    }

    logging::debug("Quering ", query_msg.get_ns(), " ", query);
/*
    base_collection::pointer collection = _engine.get_collection(query_msg.get_ns());

    assert(collection);

    collection->post(
        "query",
        params,
        [this, msg](const error_message& e, const bson_object_list& result)
        {
            if(e) {
                logging::error("Error querring ",e );
            } else {
                this->send_reply(msg, result);
            }
        });
    */

}

void connection::handle_insert_msg(const message::pointer& msg)
{
    assert(msg);
    db_message db_msg(*msg);

    bson_object_list params;
    std::string ins;
    while (db_msg.has_more()) {
        params.push_back(db_msg.next_obj());
        ins += params.back().toString();
    }

    logging::debug("inserting ", db_msg.get_ns(), " ", ins);
/*
    base_collection::pointer collection = _engine.get_collection(db_msg.get_ns());


    assert(collection);

    collection->post(
        "insert",
        params,
        [this, msg](const error_message& e, const bson_object_list& result)
        {
            if(e) {
                logging::error("Error querring ",e );
            } else {
               // this->send_reply(msg, result);
            }
        });
        */
}

std::uint32_t connection::reqId()
{
    static std::uint32_t id = 0;
    return id++;
}

void connection::send_reply(const message::pointer& msg, const bson_object_list &obj_list)
{
    message response;
    response._header._opCode = 1; // reply
    response._header._reqId = reqId();
    response._header._resId = msg->_header._reqId;
    response._header._reserved = 8;
    response._header._msglen +=  sizeof(boost::uint32_t)*2 + sizeof(boost::uint64_t);


    std::ostringstream stream(response._body, std::ios_base::out | std::ios_base::binary);

    for(const bson_object& obj: obj_list) {
        response._header._msglen += obj.objsize();
    }

    stream.write(response._header.data(), message::header_length);

    coder out(stream);

    out.put(std::uint64_t(0)); // cursor
    out.put(std::uint32_t(0)); // startingFrom
    out.put(std::uint32_t(obj_list.size())); // numberReturned

    std::string obj_string;
    for(const auto& obj: obj_list) {
        stream.write(obj.objdata(), obj.objsize());
        obj_string += obj.toString();
    }

    logging::info(
        "Sending response: ", response._header,
        " cursor: ", 0,
        " startingFrom: ", 0,
        " numberReturned: ", obj_list.size(),
        " dump: ", obj_string);

    boost::asio::async_write(
        _socket,
        boost::asio::buffer(stream.str()),
        boost::bind(&connection::handle_write_msg, shared_from_this(), boost::asio::placeholders::error));
}

void connection::handle_write_msg(const boost::system::error_code& e)
{
    if(e) {
        logging::error("Error sending response ", e.message());
    } else {
        logging::info("Response sent");
    }
}

}}}
