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

#include "frontend/mongo/collection.hpp"
#include "frontend/mongo/engine.hpp"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <boost/system/error_code.hpp>


#include <memory>


namespace falcondb { namespace frontend { namespace mongo {

collection_engine::collection_engine(falcondb::interfaces::engine& engine)
: _engine(engine)
{
}

interfaces::collection::pointer collection_engine::get_collection(const std::string& collection_name)
{
    assert(!collection_name.empty());

    std::vector<std::string> ns_splited;
    std::string ns(collection_name);
    boost::split(ns_splited, ns, boost::is_any_of("."));

    assert(!ns_splited.empty());
    if (ns_splited.size() > 1) {
        if (ns_splited[1] == "$cmd") {
            return std::make_shared<admin_collection>(_engine);
        }
    } else {
        falcondb::interfaces::database_ptr db = _engine.create_database(collection_name);
        return std::make_shared<collection>(db);
    }

    return interfaces::collection::pointer();
}

/*
void engine::result_handler(
    const std::string& operation,
    const interfaces::error_message& err,
    const document_list& data)
{
    if (err)
    {
        std::cout  << operation << " error: " << err << std::endl;
    }
    else
    {
        std::cout  << operation << " successfull" << std::endl;
        if (data.empty()) std::cout << "no data returned" << std::endl;
        else
        {
            std::cout << data.size() << " documents returned" << std::endl;
            std::copy(data.begin(), data.end(), std::ostream_iterator<document>(std::cout, "\n"));
            std::cout << std::endl;
        }
    }
}
void engine::handle_db_query(const message::pointer& msg)
{
    query_message query_msg(*msg);

    std::vector<std::string> ns_splited;
    std::string ns(query_msg.get_ns());
    boost::split(ns_splited, ns, boost::is_any_of("."));
    assert(!ns_splited.empty());

    const std::string& database_name = ns_splited[0];
    const std::string& collection_name = ns_splited[1];

    std::cerr  << "quering " << query_msg.get_ns() << std::endl;

    if (collection_name == "system") {
        assert(ns_splited.size() == 3);
        if (ns_splited[2] == "namespaces") {
            std::vector<std::string> databases = _engine.get_databases();

            bson_object_list collections;
            for(const std::string& db_name : databases) {
                std::cout << "db_name " << db_name << std::endl;
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

        std::cerr  << "Quering " << query_msg.get_ns() << " where " << query.toString() << std::endl;

        try {
            interfaces::database_ptr db = _engine.get_database(query_msg.get_ns());

            document_list doc_list;
            doc_list.push_back(document::from_json(query.jsonString()));
            db->post("list", doc_list,
                     [this, msg](const interfaces::error_message& error, const document_list& data)
                     {
                        assert(data);
                        bson_object_list object_list;
                        for(const document& doc : data) {
                            std::string jsonString = doc.to_json();
                            boost::algorithm::trim(jsonString);
                            ::mongo::BSONObj obj = ::mongo::fromjson(jsonString);
                            object_list.push_back(obj);
                        }
                        if (!error) {
                           send_reply(msg, object_list);
                        }  else {
                            std::cerr  << "Error listing db " << std::endl;
                        }
                     });
        } catch (const std::exception &e) {
            std::cerr  << "Error during db query " << e.what() << std::endl;

            send_reply(msg, bson_object_list());
        }
    }
}

void engine::handle_query_msg(const message::pointer& msg)
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

bson_object_list engine::handle_admin_command(query_message &query_msg)
{
    ::mongo::BSONObjBuilder builder;

    ::mongo::BSONObj query = query_msg.next_obj();
    std::string command(query.firstElement().fieldName());

    std::cerr  << "admin command " << command << " request" << std::endl;

    if (command == "replSetGetStatus") {
        builder.append("errmsg", "not running with --replSet");
        builder.append("ok", 1.0);
    } else if (command == "whatsmyuri") {
        builder.append("you",  _socket.remote_endpoint().address().to_string());
    } else if (command == "listDatabases") {
        std::vector<std::string> databases = _engine.get_databases();
        std::set<std::string> dbs;
        for (const std::string& database: databases) {
            dbs.insert(database.substr(0, database.find_first_of('.')));
        }

        ::mongo::BSONArrayBuilder arrayBuilder;
        for (const std::string& database: dbs) {

            ::mongo::BSONObjBuilder builder;

            builder.append("name", database);
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

bson_object_list engine::handle_db_command(const std::string& db_name, query_message& query_msg)
{
    ::mongo::BSONObjBuilder builder;

    ::mongo::BSONObj query = query_msg.next_obj();

    std::string command(query.firstElement().fieldName());
    std::cerr  << "db command " << query.toString() << std::endl;

    if (command == "getlasterror") {
        std::cerr << query.toString() << std::endl;
        builder.append("n", 1);
        builder.append("engineId", 1);
        builder.append("wtime", 0);
        builder.appendNull("err");
        builder.append("ok", double(1));
    } else if (command == "drop") {
        builder.append("ok", double(1));
    }

    return boost::assign::list_of(builder.obj());
}



void engine::send_reply(const message::pointer& msg, const bson_object_list &obj_list)
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
        boost::bind(&engine::handle_write_msg, shared_from_this(), boost::asio::placeholders::error()));
}
*/

}}}
