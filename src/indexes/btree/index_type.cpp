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

#include "utils/exception.hpp"

#include <boost/uuid/random_generator.hpp>

#include <cassert>

namespace falcondb { namespace indexes { namespace btree {

index_type::index_type()
{
}

std::unique_ptr<interfaces::index> index_type::load_index(
    interfaces::document_storage& index_storage,
    const document& index_description)
{
    const document_object& description_as_object = index_description.as_object();
    document index_root = description_as_object.get_field("root");
    document index_definition = description_as_object.get_field("definition");

    return std::unique_ptr<interfaces::index>(new index(index::load(index_storage, index_definition, index_root)));
}

interfaces::index_type::create_result index_type::create_index(
    const document& index_definition,
    interfaces::document_storage& index_storage,
    interfaces::document_storage& data_storage)
{
    verify_definition(index_definition);

    boost::uuids::random_generator gen;
    document new_storage_root = document::from(gen());
    std::unique_ptr<index> new_index(new index(index::create(index_storage, index_definition, new_storage_root)));

    document_object index_description;
    index_description.insert(std::make_pair("root", new_storage_root));
    index_description.insert(std::make_pair("definition", index_definition));

    return create_result{ document(index_description), std::move(new_index) };
}

void index_type::verify_definition(const document& definition)
{
    try
    {
        const document_list& fields = definition.as_object().get_field("fields").as_list();
        for( const document field : fields)
        {
            const document_object& field_obj = field.as_object();
            field_obj.get_field("name").as_scalar();
            field_obj.get_field("direction").as_scalar();
        }

    }
    catch(...)
    {
        throw exception("Index definition invalid");
    }
}

} } }
