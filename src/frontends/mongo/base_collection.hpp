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

#ifndef FALCONDB_FRONTEND_MONGO_BASE_COLLECTION_HPP
#define FALCONDB_FRONTEND_MONGO_BASE_COLLECTION_HPP

#include "interfaces/engine.hpp"
#include "frontends/mongo/message.hpp"

#include <functional>
#include <memory>

namespace falcondb { namespace frontend { namespace mongo {

typedef std::function<void (const error_message& error, const bson_object_list& result)> result_handler;

class base_collection
{

public:
    typedef std::shared_ptr<base_collection> pointer;

   virtual void post(
        const std::string& command,
        const bson_object_list& params,
        const result_handler& handler);

protected:

    virtual void handle_insert(
        const bson_object_list& params,
        const result_handler& handler) = 0;

    virtual void handle_query(
        const bson_object_list& params,
        const result_handler& handler) = 0;
};

}}}

#endif
