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
#include "dbengine/document_storage.hpp"

#include "indexes/btree/index_type.hpp"

#include <memory>
#include <cassert>

namespace falcondb { namespace dbengine {

database::database(const interfaces::database_backend_ptr& storage, command_processor& processor)
:
    _storage(storage),
    _processor(processor),
    _index_storage(_storage, "index"),
    _data_storage(_storage, "data")
{
    _default_index_type = std::make_shared<indexes::btree::index_type>();

    // load meta-data
    std::string key("000meta_data");
    try
    {
        std::string doc_data = _storage->get(key);
        _meta_data = document::from_json(doc_data);

        document_object index_descriptions = _meta_data.get_field("indexes").as_object();
        for(auto description : index_descriptions)
        {
            _indexes.insert(
                std::make_pair(
                    description.first,
                    _default_index_type->load_index(_index_storage, description.second)
                )
            );
        }
    }
    catch(...)
    {
        // so this is a new database
        // create main index
        document_list fields;
        document_object field1;
        field1.set_field("name", document_scalar::from(std::string("_id")));
        field1.set_field("direction", document_scalar::from(std::int32_t(1)));
        fields.push_back(field1);

        document_object options;
        options.set_field("unique", document_scalar::from(true));
        document_object definition;
        definition.set_field("fields", fields);
        definition.set_field("options", options);

        document_storage data_storage(_storage, "data");

        std::cout << "creating main index: " << definition.to_json() << std::endl;

        interfaces::index_type::create_result result = _default_index_type->create_index(
            definition,
            _index_storage,
            data_storage);

        _indexes.insert(std::make_pair("main", std::move(result.new_index)));

        // create and store meta data
        document_object index_descriptions;
        index_descriptions.insert(std::make_pair("main", result.index_description));
        _meta_data.set_field("indexes", index_descriptions);
        _storage->add(key, _meta_data.to_json());

    }
}

bool database::post(const std::string& command,
    const document& params,
    const interfaces::result_handler& result)
{
    _processor.post(command, params, result, boost::ref(*this));
    return true;
}

void database::dump()
{
    _storage->for_each(
        [&](const range& key)
        {
            std::string data = _storage->get(key);
            std::cout << key.to_string() << " => " << data << std::endl;
        });
}

} }
