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

#include "frontend/mongo/engine.hpp"

#include "interfaces/engine.hpp"

namespace falcondb { namespace frontend { namespace mongo {

class collection : public interfaces::collection
{
public:
    collection(falcondb::interfaces::database_ptr& database);

    virtual void post(
        const std::string& command,
        const bson_object_list& params,
        const interfaces::result_handler& handler);

private:

    void handle_insert(
        const bson_object_list& params,
        const interfaces::result_handler& handler);

    void handle_query(
        const bson_object_list& params,
        const interfaces::result_handler& handler);

    falcondb::interfaces::database_ptr& _databse;
};

class admin_collection : public interfaces::collection
{
public:
    admin_collection(falcondb::interfaces::engine& engine);

    virtual void post(
        const std::string& command,
        const bson_object_list& params,
        const interfaces::result_handler& handler);

private:


    void handle_query(
        const bson_object_list& params,
        const interfaces::result_handler& handler);

    falcondb::interfaces::engine& _engine;
};

}}}

#endif
