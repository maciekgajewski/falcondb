/*
FalconDB, a database
Copyright (C) 2012 Maciej Gajewski <maciej.gajewski0 at gmail dot com>

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

#include "dbengine/commands.hpp"

namespace falcondb { namespace dbengine {
namespace commands {

////////////////////////////////////////////////////
/// insert

static void insert_with_id(const interfaces::database_backend_ptr& storage, const bson_object& obj)
{
    assert(obj.hasField("_id"));

    mongo::BSONElement id = obj.getField("_id");
    range key(id.value(), id.valuesize());
    range data(obj.objdata(), obj.objsize());

    storage->add(key, data);
}

void insert(
    const bson_object& param,
    const interfaces::result_handler& handler,
    const interfaces::database_backend_ptr& storage)
{
    // does the object has _id field?
    if (!param.hasField("_id"))
    {
        // generate
        mongo::OID id;
        id.init();

        mongo::BSONObjBuilder builder;
        builder.append("_id", id);
        builder.appendElements(param);
        insert_with_id(storage, builder.obj());
    }
    else
    {
        insert_with_id(storage, param);
    }

    handler(boost::none, bson_object_list());
}

////////////////////////////////////////////////////
/// list

void list(
    const bson_object& param,
    const interfaces::result_handler& handler,
    const interfaces::database_backend_ptr& storage)
{
    bson_object_list result;
    storage->for_each(
        [&](const range& key)
        {
            //list.push_back(key.to_string());
            std::string data = storage->get(key);
            bson_object obj(data.c_str());
            result.push_back(obj.copy());
        });

    handler(boost::none, result);
}

void remove(
    const bson_object& param,
    const interfaces::result_handler& handler,
    const interfaces::database_backend_ptr& storage)
{
    storage->del(range(param.objdata(), param.objsize()));
    handler(boost::none, bson_object_list());
}


}
} }
