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

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>

namespace falcondb { namespace dbengine {
namespace commands {

////////////////////////////////////////////////////
/// insert

static void insert_with_id(const interfaces::database_backend_ptr& storage, const document& obj)
{
    assert(obj.has_field("_id"));

    document key = obj.get<document>("_id");
    std::string key_data = key.to_storage();
    std::string document_data = obj.to_storage();

    storage->add(key_data, document_data);
}

void insert(const document& param,
    const interfaces::result_handler& handler,
    const interfaces::database_backend_ptr& storage)
{
    // does the object has _id field?
    if (param.has_field("_id"))
    {
        insert_with_id(storage, param);
    }
    else
    {
        // generate
        boost::uuids::random_generator gen;
        boost::uuids::uuid id = gen();

        document copy = param;
        copy.append("_id", id);

        insert_with_id(storage, copy);
    }

    handler(boost::none, document_list());
}

////////////////////////////////////////////////////
/// list

void list(const document& param,
    const interfaces::result_handler& handler,
    const interfaces::database_backend_ptr& storage)
{
    document_list result;
    storage->for_each(
        [&](const range& key)
        {
            std::string data = storage->get(key);
            result.push_back(document::from_storage(data));
        });

    handler(boost::none, result);
}

void remove(const document& param,
    const interfaces::result_handler& handler,
    const interfaces::database_backend_ptr& storage)
{
    storage->del(param.to_storage());
    handler(boost::none, document_list());
}


}
} }
