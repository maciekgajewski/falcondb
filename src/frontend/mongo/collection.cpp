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

#include "bson/bsonobjbuilder.ipp"

#include <boost/assign/list_of.hpp>
#include <boost/system/error_code.hpp>

namespace falcondb { namespace frontend { namespace mongo {

admin_collection::admin_collection(falcondb::interfaces::engine& engine)
: _engine(engine)
{
}

void admin_collection::post(
    const std::string& command,
    const bson_object_list& params,
    const interfaces::result_handler& handler)
{
    if (command == "query") {
        handle_query(params, handler);
    } else {
        handler(interfaces::error_message(), bson_object_list());
    }
}

void admin_collection::handle_query(
    const bson_object_list& params,
    const interfaces::result_handler& handler)
{
    assert(params.size() > 0);

    ::mongo::BSONObj query = params[0];
    std::string command(query.firstElement().fieldName());

    std::cerr  << "admin command " << command << " request" << std::endl;

    ::mongo::BSONObjBuilder builder;

    if (command == "replSetGetStatus") {
        builder.append("errmsg", "not running with --replSet");
        builder.append("ok", 0.0);
    } else if (command == "whatsmyuri") {
        builder.append("you",  "127.0.0.1");
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
            builder.append("sizeOnDisk", double(1));
            builder.append("empty", true);

            arrayBuilder.append(builder.obj());
        }

        builder.appendArray("databases", arrayBuilder.obj());
        builder.append("totalSize", double(1));
        builder.append("ok", double(1));
    } if (command == "getlasterror") {
        std::cerr << query.toString() << std::endl;
        builder.append("n", 0);
        //builder.append("engineId", 1);
        builder.append("connectionId", 1);
        builder.append("wtime", 0);
        builder.appendNull("err");
    } else if (command == "drop") {
    }
    ::mongo::BSONObj result = builder.obj();
    std::cerr << "admin comman dresult " << result.toString() << std::endl;
    handler(interfaces::error_message(), boost::assign::list_of(result.copy()));
}


collection::collection(falcondb::interfaces::database_ptr& database)
: _databse(database)
{
}

void collection::post(
    const std::string& command,
    const bson_object_list& params,
    const interfaces::result_handler& handler)
{
    if (command == "insert") {
        handle_insert(params, handler);
    } else if (command == "query") {
        handle_query(params, handler);
    }
}

void collection::handle_insert(
    const bson_object_list& params,
    const interfaces::result_handler& handler)
{
    document_list doc_list;

    for(const bson_object& object: params) {
        doc_list.push_back(document::from_json(object.jsonString()));
    }

    _databse->post(
        "insert",
        doc_list[0],
        [handler](const interfaces::error_message& e, const document& result)
        {
            if (e) {
                std::cerr << *e << std::endl;
            }
            handler(e, bson_object_list());
        });
}

void collection::handle_query(
    const bson_object_list& params,
    const interfaces::result_handler& handler)
{
    return handler(interfaces::error_message(), bson_object_list());
}

}}}
