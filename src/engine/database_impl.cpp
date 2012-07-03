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

#include "engine/database_impl.hpp"

#include "engine/command_processor.hpp"

namespace falcondb { namespace engine {

database_impl::database_impl(const interfaces::database_backend_ptr& storage, command_processor& processor)
    : _storage(storage), _processor(processor)
{
}

bool database_impl::post(
    const std::string& command,
    const bson_object& params,
    const interfaces::result_handler& result)
{
    _processor.post(command, params, result, _storage);
}

} }
