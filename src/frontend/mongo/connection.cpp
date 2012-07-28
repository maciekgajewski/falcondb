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
#include "bson/json.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <boost/assign/list_of.hpp>

#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/placeholders.hpp>

#include <boost/bind.hpp>

namespace falcondb { namespace frontend { namespace mongo {

connection::connection(boost::asio::io_service& io_service, interfaces::engine& engine)
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
        boost::bind(&connection::handle_read_header, shared_from_this(), boost::asio::placeholders::error, msg));
}

void connection::handle_read_header(const boost::system::error_code& e, const message::pointer& msg)
{
    if (e)
    {
        std::cerr << "Error reading header: " << e << std::endl;
    }
    else
    {
        const message::header &header = msg->_header;
        std::string &body = msg->_body;

        std::cerr << "Header read: " << header << std::endl;

        body.resize(header._msglen - sizeof(header));

        std::cerr << "Reading body: " << body.size() << std::endl;

        boost::asio::async_read(
            _socket,
            boost::asio::buffer(const_cast<char*>(body.data()), body.size()),
            boost::bind(&connection::handle_read_body, shared_from_this(), boost::asio::placeholders::error, msg));
        }
}

bson_object_list connection::handle_admin_command(query_message &query_msg)
{
    ::mongo::BSONObjBuilder builder;

    ::mongo::BSONObj query = query_msg.next_obj();
    std::string command(query.firstElement().fieldName());

    std::cerr << "admin command " << command << " request" << std::endl;

    if (command == "replSetGetStatus") {
        builder.append("errmsg", "not running with --replSet");
        builder.append("ok", 1.0);
    } else if (command == "whatsmyuri") {
        builder.append("you",  _socket.remote_endpoint().address().to_string());
    } else if (command == "listDatabases") {
        std::vector<std::string> databases = _engine.get_databases();
        ::mongo::BSONArrayBuilder arrayBuilder;
        for (const std::string& database: databases) {

            ::mongo::BSONObjBuilder builder;

            std::size_t dot_pos = database.find_first_of('.');
            if (dot_pos != std::string::npos) {
                builder.append("name", database.substr(0, dot_pos));
            } else {
                builder.append("name", database);
            }

            builder.append("sizeOnDisk", 1);
            builder.append("empty", true);

            arrayBuilder.append(builder.obj());
        }
        builder.appendArray("databases", arrayBuilder.obj());
        builder.append("totalSize", double(1));
        builder.append("ok", double(1));
    }
    return boost::assign::list_of(builder.obj());
}

bson_object_list connection::handle_db_command(const std::string& db_name, query_message& query_msg)
{
    ::mongo::BSONObjBuilder builder;

    ::mongo::BSONObj query = query_msg.next_obj();

    std::string command(query.firstElement().fieldName());
    std::cerr << "db command " << command << " request" << std::endl;

    if (command == "getlasterror") {
        std::cerr << query.toString() << std::endl;
        builder.append("n", 1);
        builder.append("connectionId", 1);
        builder.append("wtime", 0);
        builder.appendNull("err");
        builder.append("ok", double(1));
    }

    return boost::assign::list_of(builder.obj());
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

    std::cerr << "sending response: " << response._header << std::endl;

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
        boost::bind(&connection::handle_write_msg, shared_from_this(), boost::asio::placeholders::error));
}

void connection::handle_db_query(const message::pointer& msg)
{
    query_message query_msg(*msg);

    std::vector<std::string> ns_splited;
    std::string ns(query_msg.get_ns());
    boost::split(ns_splited, ns, boost::is_any_of("."));
    assert(!ns_splited.empty());

    const std::string& database_name = ns_splited[0];
    const std::string& collection_name = ns_splited[1];

    if (collection_name == "system") {
        assert(ns_splited.size() == 3);
        if (ns_splited[2] == "namespaces") {
            std::vector<std::string> databases = _engine.get_databases();

            bson_object_list collections;
            for(const std::string& db_name : databases) {

                if( db_name.find(database_name + ".") == 0) {
                    ::mongo::BSONObjBuilder builder;
                    builder.append("name", db_name);
                    collections.push_back(builder.obj());
                }
            }
            send_reply(msg, collections);
        }
    } else {
        ::mongo::BSONObj query;

        if (query_msg.has_more()) query = query_msg.next_obj();

        std::cerr << "Quering " << query_msg.get_ns() << " where " << query.toString() << std::endl;

        try {
            interfaces::database_ptr db = _engine.get_database(query_msg.get_ns());

            db->post("list", documents::from_json(query.jsonString()),
                     [this, msg](const interfaces::error_message& error, const document_list& data)
                     {
                        bson_object_list object_list;
                        for(const document& doc : data) {
                            std::string jsonString = documents::to_json(doc);
                            boost::algorithm::trim(jsonString);
                            ::mongo::BSONObj obj = ::mongo::fromjson(jsonString);
                            object_list.push_back(obj);
                        }
                        if (!error) {
                           send_reply(msg, object_list);
                        }  else {
                            std::cerr << "Error listing db " << std::endl;
                        }
                     });
        } catch (const std::exception &e) {
            std::cerr << "Error during db query " << e.what() << std::endl;
        }
    }
}

void connection::handle_query_msg(const message::pointer& msg)
{
    query_message query_msg(*msg);

    std::vector<std::string> ns_splited;
    std::string ns(query_msg.get_ns());
    boost::split(ns_splited, ns, boost::is_any_of("."));
    assert(!ns_splited.empty());

    const std::string& database_name = ns_splited[0];
    const std::string& collection_name = ns_splited[1];

    if (collection_name == "$cmd") {
        if (database_name == "admin") {
            send_reply(msg, handle_admin_command(query_msg));
        } else {
            send_reply(msg, handle_db_command(database_name, query_msg));
        }
    } else {
        handle_db_query(msg);
    }
}

void connection::post_command(const std::string& db_name, const std::string& command, const document& param)
{
    interfaces::database_ptr db = _engine.get_database(db_name);

    db->post(command, param,
             [this, command](const interfaces::error_message& error, const document_list& data)
             {
                result_handler(command, error, data);
            });
}

void connection::result_handler(
    const std::string& operation,
    const interfaces::error_message& err,
    const document_list& data)
{
    if (err)
    {
        std::cout << operation << " error: " << err << std::endl;
    }
    else
    {
        std::cout << operation << " successfull" << std::endl;
        if (data.empty()) std::cout << "no data returned" << std::endl;
        else
        {
            std::cout << data.size() << " documents returned" << std::endl;
            std::copy(data.begin(), data.end(), std::ostream_iterator<document>(std::cout, "\n"));
            std::cout << std::endl;
        }
    }
}

void connection::handle_insert_msg(const message::pointer& msg)
{
    db_message dbmsg(*msg);

    std::string ns(dbmsg.get_ns());

    try {
        _engine.create_database(ns);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    while (dbmsg.has_more()) {
        std::string jsonString = dbmsg.next_obj().jsonString() ;
        post_command(
            ns,
            "insert",
            documents::from_json(jsonString));
    }
}

void connection::handle_read_body(const boost::system::error_code& e, const message::pointer& msg)
{
    if (e) {
        std::cerr << "Error reading body: " << e << std::endl;
    } else {
        std::cerr << "Body read: " << std::endl;

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

void connection::handle_write_msg(const boost::system::error_code& e)
{
    if(e) {
        std::cerr << "Error sending response " << e << std::endl;
    } else {
        std::cerr << "Response sent" << std::endl;
    }
}

}}}
