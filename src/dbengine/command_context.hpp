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

#ifndef FALCONDB_DBENGINE_COMMAND_CONTEXT_HPP
#define FALCONDB_DBENGINE_COMMAND_CONTEXT_HPP

#include "dbengine/document_storage.hpp"

#include "interfaces/command_context.hpp"

namespace falcondb { namespace dbengine {

class command_context : public interfaces::command_context
{
public:
    command_context(
        const document_storage& data_storage,
        interfaces::index_map& indexes
        );

    // interface

    virtual interfaces::document_storage& get_data_storage();
    virtual interfaces::index_map& get_indexes();

private:

    document_storage _data_storage;
    interfaces::index_map& _indexes;
};

} }

#endif // FALCONDB_DBENGINE_COMMAND_CONTEXT_HPP
