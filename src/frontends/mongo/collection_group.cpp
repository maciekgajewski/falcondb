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

#include "frontends/mongo/collection_group.hpp"

#include "utils/log.hpp"

#include "bson/bsonobjbuilder.ipp"

#include <boost/assign/list_of.hpp>
#include <boost/system/error_code.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <boost/bind.hpp>
#include <boost/bind/placeholders.hpp>

namespace falcondb { namespace frontend { namespace mongo {

collection_group::collection_group(const std::vector<std::string>& ns, falcondb::interfaces::engine& engine)
: _ns(ns), _engine(engine)
{
}

std::vector<std::string> collection_group::get_collection_names()
{
    typedef std::vector<std::string> collection_name_list;
    //collection_name_list collection_names = _engine.get_databases();

    const std::string& group_name = _ns[0];
    collection_name_list::iterator end =
    std::remove_if(
        collection_names.begin(),
        collection_names.end(),
        [&group_name](const std::string& collection_name)
        {
            return !boost::algorithm::starts_with(collection_name, (group_name+'.'));
        });
    return std::vector<std::string>(collection_names.begin(), end);
}

void collection_group::handle_query(const bson_object_list& params, const result_handler& handler)
{
    bson_object_list result;
    assert(_ns.size()>1);
    const std::string& group_name = _ns[0];
    const std::string& collection_name = _ns[1];

    if (collection_name == "system") {
        assert(_ns.size() > 2);
        if (_ns[2] == "namespaces") {
            for (const std::string& coll_name: get_collection_names()) {
                ::mongo::BSONObjBuilder builder;
                builder.append("name", coll_name);
                result.push_back(builder.obj());
            }
        }
    } else if (_ns[1] == "$cmd") {
        assert(!params.empty());
        ::mongo::BSONObj query = params[0];
        std::string command(query.firstElement().fieldName());

        if (command == "dropDatabase") {
            for(const auto& coll_name : get_collection_names()) {
                logging::info("dropping ", coll_name);
                _engine.drop_database(coll_name);
                logging::info(coll_name, " dropped");
            }
        } else if (command == "getlasterror") {
            ::mongo::BSONObjBuilder builder;
            builder.append("n", std::int32_t(0));
            builder.append("connectionId", std::int32_t(0));
            builder.append("wtime", std::int32_t(0));
            builder.appendNull("err");
            builder.append("ok", double(1.0));
            result.push_back(builder.obj());
        }

        logging::debug("db ", group_name, " command ", command, " request");
    }

    handler(error_message(), result);
}

void collection_group::handle_insert(const bson_object_list& params, const result_handler& handler)
{
}

}}}
