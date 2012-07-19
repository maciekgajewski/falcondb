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

    return new index(index_storage, index_definition, index_root);
}

document index_type::create_index(
    const document& index_definition,
    interfaces::index_iterator& documents,
    interfaces::document_storage& index_storage)
{
}

} } }
