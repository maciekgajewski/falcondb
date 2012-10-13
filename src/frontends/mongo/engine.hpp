#ifndef FALCONDB_FRONTEND_MONGO_ENGINE_HPP
#define FALCONDB_FRONTEND_MONGO_ENGINE_HPP

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

#include "interfaces/engine.hpp"

#include "frontends/mongo/message.hpp"

#include "frontends/mongo/base_collection.hpp"

#include <boost/optional.hpp>

#include <string>
#include <functional>
#include <vector>

namespace falcondb { namespace frontend { namespace mongo {

class collection_engine
{

public:

    collection_engine(falcondb::interfaces::engine& engine);

    virtual base_collection::pointer get_collection(const std::string& collection_name);

private:

    falcondb::interfaces::engine& _engine;
};

}}}

#endif
