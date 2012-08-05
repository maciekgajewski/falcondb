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

static void insert_with_id(interfaces::command_context& context, const document_object& doc)
{
    assert(doc.has_field("_id"));

    document key = doc.get_field("_id");

    context.get_data_storage().write(key, doc);

    // update indexes
    for( const auto& index : context.get_indexes())
    {
        index.second->insert(key, doc);
    }
}

void insert(const document& param,
    const interfaces::result_handler& handler,
    interfaces::command_context& context)
{
    const document_object& as_obj = param.as_object();
    // does the object has _id field?
    if (as_obj.has_field("_id"))
    {
        insert_with_id(context, as_obj);
    }
    else
    {
        // generate
        boost::uuids::random_generator gen;
        boost::uuids::uuid id = gen();

        document_object copy = as_obj;
        copy.set_field("_id", document_scalar::from(id));

        insert_with_id(context, copy);
    }

    handler(error_message(), document_list());
}

////////////////////////////////////////////////////
/// list

void list(const document& param,
    const interfaces::result_handler& handler,
    interfaces::command_context& context)
{
    // TODO
    // list should use low-level scan on document_storage, to be inmplemented
    handler(error_message("'list' not implemented", backtrace_data::create()), document_list());
}


void remove(const document& param,
    const interfaces::result_handler& handler,
    interfaces::command_context& context)
{
    // remove from indexes
    document doc = context.get_data_storage().read(param);
    for(const auto& index : context.get_indexes())
    {
        index.second->del(doc);
    }

    context.get_data_storage().remove(param); // the param is the key
    handler(error_message(), document_list());
}

void listindexes(const document& param,
    const interfaces::result_handler& handler,
    interfaces::command_context& context)
{
    document_list result;
    const interfaces::index_map& indexes = context.get_indexes();

    result.reserve(indexes.size());
    std::transform(
        indexes.begin(), indexes.end(),
        std::back_inserter(result),
        [] (const interfaces::index_map::value_type& p) { return document_scalar::from(p.first); });

    handler(error_message(), result);
}


} // namespace commands
} }
