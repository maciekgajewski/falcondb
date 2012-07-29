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

    virtual document next() { assert(false); return document_scalar(null_type()); }
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
        _meta_data = document::from_json(doc_data);

        document_list index_descriptions = _meta_data.get_field("indexes").as_list();
        for(const document& description : index_descriptions)
        {
            _indexes.push_back(_default_index_type->load_index(_index_storage, description));
        }
    }
    catch(...)
    {
        // so this is a new database
        // create main index
        document_object fields;
        fields.set_field("_id", 1);
        document_object options;
        options.set_field("unique", true);
        document_object definition;
        definition.set_field("fields", fields);
        definition.set_field("options", options);

        null_index_iterator it;

        document_storage data_storage(_storage, "data");

        std::cout << "creating main index: " << definition.to_json() << std::endl;

        interfaces::index_type::create_result result = _default_index_type->create_index(
            definition,
            it,
            _index_storage,
            data_storage);

        _indexes.push_back(std::move(result.new_index));

        // create and store meta data
        document_list index_descriptions;
        index_descriptions.push_back(result.index_description);
        _meta_data.set_field("indexes", index_descriptions);
        _storage->add(key, _meta_data.to_json());

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
