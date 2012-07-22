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

#include "dbengine/database.hpp"

#include "dbengine/command_processor.hpp"
#include "dbengine/command_context.hpp"
#include "dbengine/document_storage.hpp"

#include "indexes/btree/index_type.hpp"

#include <memory>
#include <cassert>

namespace falcondb { namespace dbengine {

class null_index_iterator : public interfaces::index_iterator
{
public:
    virtual bool has_next() { return false; }

    virtual document next() { assert(false); return document(); }
};

database::database(const interfaces::database_backend_ptr& storage, command_processor& processor)
    : _storage(storage), _processor(processor), _index_storage(_storage, "index")
{
    _default_index_type = std::make_shared<indexes::btree::index_type>();

    // load meta-data
    std::string key("000meta_data");
    try
    {
        std::string doc_data = _storage->get(key);
        _meta_data = document::from_storage(doc_data);

        document index_descriptions = _meta_data.get<document>("indexes");
        for(const document& description : index_descriptions)
        {
            _indexes.push_back(_default_index_type->load_index(_index_storage, description));
        }
    }
    catch(...)
    {
        // so this is a new database
        // create main index
        document fields;
        fields.append("_id", 1);
        document options;
        options.append("unique", true);
        document definition;
        definition.append("fields", fields);
        definition.append("options", options);

        null_index_iterator it;

        document_storage data_storage(_storage, "data");

        interfaces::index_type::create_result result = _default_index_type->create_index(
            definition,
            it,
            _index_storage,
            data_storage);

        _indexes.push_back(std::move(result.new_index));

        // create and store meta data
        document index_descriptions = document::empty_array();
        index_descriptions.append(result.index_description);
        _meta_data.append("indexes", index_descriptions);
        _storage->add(key, _meta_data.to_storage());

    }
}

bool database::post(const std::string& command,
    const document& params,
    const interfaces::result_handler& result)
{
    // build context for the command
    document_storage data_storage(_storage, "data");
    command_context context(data_storage, _indexes);

    _processor.post(command, params, result, context);
    return true;
}

} }
