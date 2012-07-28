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

#include "indexes/btree/index_iterator.hpp"

#include "interfaces/document_storage.hpp"

namespace falcondb { namespace indexes { namespace btree {

index_iterator::index_iterator(const document& leaf_node, std::size_t index, interfaces::document_storage& storage)
:   _leaf_node(leaf_node), _index(index), _storage(storage)
{
}

bool index_iterator::has_next()
{
    const document_array& data = _leaf_node.get_field_as_array("data");
    return _index < data.size() || _leaf_node.as_map().find("next") != _leaf_node.as_map().end();
}

document index_iterator::next()
{
    const document_array& data = _leaf_node.get_field_as_array("data");
    if (_index < data.size())
    {
        return document(data[_index++]).get_field_as<document>("value");
    }
    else
    {
        _index = 0;
        document next_key = _leaf_node.get_field_as<document>("next");
        _leaf_node = _storage.read(next_key);
        return document(_leaf_node.get_field_as_array("data")[0]).get_field_as<document>("value");
    }
}

} } }
