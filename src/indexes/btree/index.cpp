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
#include <limits>
#include <cstdint>

static std::size_t ITEMS_PER_LEAF = 100; // completely arbitrary

namespace falcondb { namespace indexes { namespace btree {

index index::create(interfaces::document_storage& storage, const document& definition, const document& root_storage_key)
{
    const document_object def_obj = definition.as_object();
    bool unique = false;
    if (def_obj.has_field("unique"))
        unique = def_obj.get_field("unique").as<bool>();

    btree tree = btree::create(storage, root_storage_key, unique, ITEMS_PER_LEAF);
    return index(std::move(tree), def_obj);
}

index index::load(interfaces::document_storage& storage, const document& definition, const document& root_storage_key)
{
    const document_object def_obj = definition.as_object();
    bool unique = false;
    if (def_obj.has_field("unique"))
        unique = def_obj.get_field("unique").as<bool>();

    btree tree = btree::load(storage, root_storage_key, unique, ITEMS_PER_LEAF);
    return index(std::move(tree), def_obj);
}


index::index(btree&& tree, const document_object& definition)
:
    _tree(std::move(tree)),
    _fields()
{
    // read the fields definition
    const document_list& fields = definition.get_field("fields").as_list();
    for(const document& field : fields)
    {
        const document_object& field_obj = field.as_object();
        std::string field_name = field_obj.get_field("name").as_scalar().as<std::string>();
        std::int32_t direction = field_obj.get_field("direction").as_scalar().as<std::int32_t>();
        _fields.insert(std::make_pair(field_name, direction));
    }
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
    bool min_inclusive,
    const boost::optional<document>& max,
    bool max_inlcusive,
    const boost::optional<std::size_t> limit,
    const boost::optional<std::size_t> skip)
{
    boost::optional<document_list> min_index_key;
    boost::optional<document_list> max_index_key;
    if (min) min_index_key = extract_index_key(*min);
    if (max) max_index_key = extract_index_key(*max);

    std::size_t s = 0;
    if (skip) s = *skip;
    std::size_t l = std::numeric_limits<std::size_t>::max();
    if (limit) l = *limit;

    return _tree.scan(min_index_key, min_inclusive, max_index_key, max_inlcusive, l, s);
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
