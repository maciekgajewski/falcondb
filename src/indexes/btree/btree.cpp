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

#include "indexes/btree/btree.hpp"

#include <boost/uuid/random_generator.hpp>

namespace falcondb { namespace indexes { namespace btree {

btree load(interfaces::document_storage& storage,  const document& root_storage_key)
{
    return btree(storage, root_storage_key);
}

btree create(interfaces::document_storage& storage,  const document& root_storage_key)
{
    document root_doc = create_leaf();
    storage.write(root_storage_key, root_doc);

    return btree(storage, root_storage_key);
}


btree::btree(interfaces::document_storage& storage, const document& root_storage_key)
: _storage(storage), _root_storage_key(root_storage_key)
{
}

document_list btree::scan(
    const boost::optional<document_list>& low,
    const boost::optional<document_list>& hi,
    std::size_t limit,
    std::size_t skip)
{
    // TODO implement
    assert(false);
    return document_list();
}

void btree::insert(const document_list key, const document& value)
{
    tree_insert(_root_storage_key, key, value);
}

void btree::remove(const document_list& key)
{
    tree_remove(_root_storage_key, key);
}

document btree::generate_key()
{
    boost::uuids::random_generator gen;
    return document_scalar::from(gen());
}

document btree::create_leaf()
{
    document_object leaf;
    leaf.insert(std::make_pair("type", document::from("leaf")));
    leaf.insert(std::make_pair("data", document_list()));

    return document(leaf);
}

void btree::tree_insert(const document& node_key, const document_list& key, const document& value)
{
    document_object node = _storage.read(node_key);

    if(node.get_field("type").as_scalar().as<std::string>() == "leaf")
    {
        tree_insert_leaf(node_key, node, key, value);
    }
    else
    {
        tree_insert_interior(node_key, node, key, value);
    }
}

void btree::tree_insert_leaf(
    const document& node_key,
    document_object& node,
    const document_list& key,
    const document& value)
{
    document_list& data = node.get_field("data").as_list();
    // will fit?
    if (data.size() < ITEMS_PER_LEAF)
    {
        document_object new_element;
        new_element.insert(std::make_pair("key", key));
        new_element.insert(std::make_pair("value", value));

        auto it = std::lower_bound(
            data.begin(), data.end(),
            new_element,
            [this](const document& a, const document& b)
            {
                return
                    a.as_object().get_field("key").as_list() <
                    b.as_object().get_field("key").as_list();
            });

        data.insert(it, new_element);

        _storage.write(node_key, node);
    }
    else
    {
        // split leaf, returns something to say the parent that it should update
        assert(false); // TODO implement!
    }
}

void btree::tree_insert_interior(
    const document& node_key,
    document_object& node,
    const document_list& key,
    const document& value)
{
    // TODO implement
    assert(false);
}

void btree::tree_remove(const document& node_storage_key, const document_list& key)
{
    document_object node = _storage.read(node_storage_key);

    if(node.get_field("type").as_scalar().as<std::string>() == "leaf")
    {
        tree_remove_leaf(node_storage_key, node, key);
    }
    else
    {
        tree_remove_interior(node_storage_key, node, key);
    }
}

void btree::tree_remove_leaf(
    const document& node_storage_key,
    document_object& node,
    const document_list& key)
{
    document_list& data = node.get_field("data").as_list();

    document_object search;
    search.insert(std::make_pair("key", key));

    auto range = std::equal_range(
        data.begin(), data.end(),
        search,
        [this](const document& a, const document& b)
        {
            return
                a.as_object().get_field("key").as_list() <
                b.as_object().get_field("key").as_list();
        });

    assert(range.first != range.second);
    data.erase(range.first, range.second);

    _storage.write(node_storage_key, node);

    if (data.size() < ITEMS_PER_LEAF/2 )
    {
        // consolidate nodes or what?
        assert(false); // TODO implement
    }
}

void btree::tree_remove_interior(
    const document& node_key,
    document_object& node,
    const document_list& key)
{
    // TODO implement
    assert(false);
}

}}} // ns
