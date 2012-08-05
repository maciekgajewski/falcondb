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

#ifndef FALCONDB_FRONTEND_MONGO_COLLECTION_HPP
#define FALCONDB_FRONTEND_MONGO_COLLECTION_HPP

#include "frontend/mongo/base_collection.hpp"

#include "frontend/mongo/engine.hpp"

namespace falcondb { namespace frontend { namespace mongo {

class collection : public base_collection
{
public:

    collection(const std::string& name, falcondb::interfaces::database_ptr& database);

private:

    virtual void handle_insert(
        const bson_object_list& params,
        const result_handler& handler);

    virtual void handle_query(
        const bson_object_list& params,
        const result_handler& handler);

    const std::string _name;
    falcondb::interfaces::database_ptr _databse;
};


}}}

#endif
