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

#include "interfaces/document_storage.hpp"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>

namespace falcondb { namespace dbengine {
namespace commands {

////////////////////////////////////////////////////
/// insert

static void insert_with_id(database& db, const document_object& doc)
{
    assert(doc.has_field("_id"));

    document key = doc.get_field("_id");

    db.get_data_storage().write(key, doc);

    // update indexes
    for( const auto& index : db.get_indexes())
    {
        index.second->insert(key, doc);
    }
}

void insert(const document& param,
    const interfaces::result_handler& handler,
    database& db)
{
    const document_object& as_obj = param.as_object();
    // does the object has _id field?
    if (as_obj.has_field("_id"))
    {
        insert_with_id(db, as_obj);
    }
    else
    {
        // generate
        boost::uuids::random_generator gen;
        boost::uuids::uuid id = gen();

        document_object copy = as_obj;
        copy.set_field("_id", document_scalar::from(id));

        insert_with_id(db, copy);
    }

    handler(error_message(), document_list());
}

////////////////////////////////////////////////////
/// list

void list(const document& param,
    const interfaces::result_handler& handler,
    database& db)
{
    document_list result;

    db.get_data_storage().for_each(
        [&result](const document& key, const document& value)
        {
            result.push_back(value);
        });

    handler(error_message(), result);
}

////////////////////////////////////////////////////
/// remove

void remove(const document& param,
    const interfaces::result_handler& handler,
    database& db)
{
    // remove from indexes
    document doc = db.get_data_storage().read(param);
    for(const auto& index : db.get_indexes())
    {
        index.second->del(doc);
    }

    db.get_data_storage().remove(param); // the param is the key
    handler(error_message(), document_list());
}

////////////////////////////////////////////////////
/// listindexes

void listindexes(const document& param,
    const interfaces::result_handler& handler,
    database& db)
{
    document_list result;
    const database::index_map& indexes = db.get_indexes();

    result.reserve(indexes.size());
    std::transform(
        indexes.begin(), indexes.end(),
        std::back_inserter(result),
        [] (const database::index_map::value_type& p) { return document_scalar::from(p.first); });

    handler(error_message(), result);
}


} // namespace commands
} }
