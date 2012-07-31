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

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <boost/assign/list_of.hpp>

#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/placeholders.hpp>

#include <boost/bind.hpp>

namespace falcondb { namespace frontend { namespace mongo {

connection::connection(boost::asio::io_service& io_service, interfaces::collection_engine& engine)
: _socket(io_service)
, _engine(engine)
{
}

void connection::start()
{
    message::pointer msg = std::make_shared<message>();
    std::cerr << "Reading header: " << message::header_length << std::endl;

    boost::asio::async_read(
        _socket,
        boost::asio::buffer(const_cast<char*>(msg->_header.data()), message::header_length),
        boost::bind(&connection::handle_read_header, shared_from_this(), boost::asio::placeholders::error(), msg));
}

void connection::handle_read_header(const boost::system::error_code& e, const message::pointer& msg)
{
    if (e) {
        std::cerr << "Error reading header: " << e << std::endl;
    } else {
        const message::header &header = msg->_header;
        std::string &body = msg->_body;

        std::cerr  << "Header read: " << header << std::endl;

        body.resize(header._msglen - sizeof(header));

        std::cerr  << "Reading body: " << body.size() << std::endl;

        boost::asio::async_read(
            _socket,
            boost::asio::buffer(const_cast<char*>(body.data()), body.size()),
            boost::bind(&connection::handle_read_body, shared_from_this(), boost::asio::placeholders::error(), msg));
    }
}

void connection::handle_read_body(const boost::system::error_code& e, const message::pointer& msg)
{
    if (e) {
        std::cerr << "Error reading body: " << e << std::endl;
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

    interfaces::collection::pointer collection = _engine.get_collection(query_msg.get_ns());

    assert(collection);
    bson_object_list params;
    while (query_msg.has_more()) {
        params.push_back(query_msg.next_obj().copy());
    }

    collection->post(
        "query",
        params,
        [this, msg](const interfaces::error_message& e, const bson_object_list& result)
        {
            if(e) {
                std::cerr << *e << std::endl;
            } else {
                this->send_reply(msg, result);
            }
        });
}

void connection::handle_insert_msg(const message::pointer& msg)
{
}

void connection::send_reply(const message::pointer& msg, const bson_object_list &obj_list)
{
    message response;
    response._header._opCode = 1; // reply
    response._header._reqId = 1;
    response._header._resId = msg->_header._reqId;
    response._header._msglen
        +=  sizeof(boost::uint32_t)*2
        + sizeof(boost::uint64_t);


    std::ostringstream stream(response._body, std::ios_base::out | std::ios_base::binary);

    for(const bson_object& obj: obj_list) {
        response._header._msglen += obj.objsize();
    }

    stream.write(response._header.data(), message::header_length);

    std::cerr  << "sending response: " << response._header << std::endl;

    coder out(stream);

    out.put(std::uint64_t(0)); // cursor
    out.put(std::uint32_t(0)); // startingFrom
    out.put(std::uint32_t(obj_list.size())); // numberReturned

    for(const bson_object& obj: obj_list) {
        stream.write(obj.objdata(), obj.objsize());
    }

    boost::asio::async_write(
        _socket,
        boost::asio::buffer(stream.str()),
        boost::bind(&connection::handle_write_msg, shared_from_this(), boost::asio::placeholders::error()));
}


void connection::handle_write_msg(const boost::system::error_code& e)
{
    if(e) {
        std::cerr << "Error sending response " << e.message() << std::endl;
    } else {
        std::cerr << "Response sent" << std::endl;
    }
}

}}}
