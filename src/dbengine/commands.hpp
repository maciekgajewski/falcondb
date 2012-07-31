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

#ifndef FALCONDB_DBENGINE_COMMANDS_COMMANDS_HPP
#define FALCONDB_DBENGINE_COMMANDS_COMMANDS_HPP

#include "interfaces/engine.hpp"
#include "interfaces/command_context.hpp"

namespace falcondb { namespace dbengine {
namespace commands {

/// upsert object into collection. _id is added to the object if absent
void insert(const document& param,
    const interfaces::result_handler& handler,
    interfaces::command_context& context);

// returns the entire content of the collection in no particular order
void list(
    const document& param,
    const interfaces::result_handler& handler,
    interfaces::command_context& context);

// removes object which _id is equal to param
void remove(const document& param,
    const interfaces::result_handler& handler,
    interfaces::command_context& context);

// dumps the entire db content ot stdout
void dump(const document& param,
    const interfaces::result_handler& handler,
    interfaces::command_context& context);


}
} }

#endif
