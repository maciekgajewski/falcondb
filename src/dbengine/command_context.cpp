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

#include "dbengine/command_context.hpp"

namespace falcondb { namespace dbengine {

command_context::command_context(
        const document_storage& data_storage,
        std::vector<interfaces::index::unique_ptr>& indexes
        )
    : _data_storage(data_storage), _indexes(indexes)
{
}

interfaces::document_storage& command_context::get_data_storage()
{
    return _data_storage;
}

std::vector<interfaces::index::unique_ptr>&  command_context::get_indexes()
{
    return _indexes;
}

} }