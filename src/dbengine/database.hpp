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

#ifndef FLACONDB_ENGINE_DATABASE_IMPL_HPP
#define FLACONDB_ENGINE_DATABASE_IMPL_HPP

#include "interfaces/engine.hpp"
#include "interfaces/storage_backend.hpp"
#include "interfaces/index.hpp"

#include "dbengine/document_storage.hpp"

namespace falcondb { namespace dbengine {

class command_processor;

class database : public interfaces::database
{
public:
    database(const interfaces::database_backend_ptr& storage, command_processor& processor);

    virtual bool post(
        const std::string& command,
        const document& params,
        const interfaces::result_handler& result);

    virtual void dump();

    // interface for own commands
    typedef std::map<std::string, interfaces::index::unique_ptr> index_map;
    index_map& get_indexes() { return _indexes; }
    document_storage& get_data_storage() { return _data_storage; }


private:

    interfaces::database_backend_ptr _storage;
    command_processor& _processor;
    document_object _meta_data;

    index_map _indexes;
    document_storage _index_storage;

    document_storage _data_storage;

    interfaces::index_type::pointer _default_index_type;

};

} }

#endif
