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

#include "utils/log.hpp"

#include "bson/bsonobjbuilder.ipp"

#include <boost/assign/list_of.hpp>
#include <boost/system/error_code.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <boost/bind.hpp>
#include <boost/bind/placeholders.hpp>

namespace falcondb { namespace frontend { namespace mongo {

collection::collection(const std::string& name, falcondb::interfaces::database_ptr& database)
: _name(name), _databse(database)
{
    assert(_databse);
}

void collection::handle_insert(
    const bson_object_list& params,
    const result_handler& handler)
{
    auto result_handler =
        [handler](const error_message& e, const document& result)
        {
            if (e) {
                logging::error("Error inserting ", e);
            } else {
                logging::info("Inserted ");
                handler(e, bson_object_list());
            }
        };

    document_list doc_list;
    for(const bson_object& object: params) {
        doc_list.push_back(document::from_json(object.jsonString()));
    }

    _databse->post( "insert", doc_list, result_handler);
}

void collection::handle_query(
    const bson_object_list& params,
    const result_handler& handler)
{
    return handler(error_message(), bson_object_list());
}




}}}
