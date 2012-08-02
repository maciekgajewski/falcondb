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

#include "indexes/btree/index.hpp"

#include "interfaces/document_storage.hpp"

#include <algorithm>

namespace falcondb { namespace indexes { namespace btree {

index index::create(interfaces::document_storage& storage, const document& definition, const document& root_storage_key)
{
    btree tree = btree::create(storage, root_storage_key);
    return index(std::move(tree), definition);
}

index index::load(interfaces::document_storage& storage, const document& definition, const document& root_storage_key)
{
    btree tree = btree::load(storage, root_storage_key);
    return index(std::move(tree), definition);
}


index::index(btree&& tree, const document& definition)
:
    _tree(std::move(tree)),
    _fields(definition.as_object().get_field("fields").as_object().to_map_of<int>())
{
}

index::index(index&& other)
:
    _tree(std::move(other._tree)),
    _fields(std::move(other._fields))
{
}

index::~index()
{
}

void index::insert(const document& storage_key, const document& doc)
{
    document_list index_key = extract_index_key(doc);

    // enter the actual recursive algo
    _tree.insert(index_key, storage_key);

}

void index::update(const document& old_doc, const document& new_doc)
{
    // TODO
    assert(false);
}

void index::del(const document& doc)
{
    document_list index_key = extract_index_key(doc);
    _tree.remove(index_key);
}

document_list index::scan(
    const boost::optional<document>& min,
    const boost::optional<document>& max,
    const boost::optional<std::size_t> limit,
    const boost::optional<std::size_t> skip)
{
    boost::optional<document_list> min_index_key;
    boost::optional<document_list> max_index_key;
    if (min) min_index_key = extract_index_key(*min);
    if (max) max_index_key = extract_index_key(*max);

    return _tree.scan(min_index_key, max_index_key, limit, skip);
}

document_list index::extract_index_key(const document& doc)
{
    const document_object& as_map = doc.as_object();
    document_list result;
    result.reserve(_fields.size());

    for(auto field : _fields)
    {
        auto it = as_map.find(field.first);
        if (it == as_map.end())
        {
            result.push_back(document_scalar::null());
        }
        else
        {
            result.push_back(it->second);
        }
    }

    return result;
}


} } }
