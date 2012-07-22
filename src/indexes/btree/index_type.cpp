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

#include "indexes/btree/index_type.hpp"
#include "indexes/btree/index.hpp"

#include "interfaces/document_storage.hpp"

#include <cassert>

namespace falcondb { namespace indexes { namespace btree {

index_type::index_type()
{
}

std::unique_ptr<interfaces::index> index_type::load_index(
    interfaces::document_storage& index_storage,
    const document& index_description)
{
    document index_root = index_description.get<document>("root");
    document index_definition = index_description.get<document>("definition");

    assert(!index_root.is_null());
    assert(!index_definition.is_null());

    return std::unique_ptr<interfaces::index>(new index(index_storage, index_definition, index_root));
}

document index_type::create_index(
    const document& index_definition,
    interfaces::index_iterator& documents,
    interfaces::document_storage& index_storage,
    interfaces::document_storage& data_storage)
{
    verify_definition(index_definition);

    std::unique_ptr<index> new_index(new index(index_storage, index_definition));
    // copy data
    while(documents.has_next())
    {
        document storage_key = documents.next();
        document doc = data_storage.read(storage_key);

        new_index->insert(storage_key, doc);
    }


    document index_description;
    index_description.append("root", new_index->get_root());
    index_description.append("definition", index_definition);

    return document();
}

void index_type::verify_definition(const document& definition)
{
    document fields = definition.get<document>("fields");
    if (fields.is_null())
        throw exception("fields not defined");

    //document options = definiton.get<document>("options");
    // options are optional, no need to check

    // feilds has top be a flat doc with non-zero integers
    for(const document& field : fields)
    {
        if (field.as<int>() == 0)
            throw exception("field can not be 0");
    }
}

} } }
