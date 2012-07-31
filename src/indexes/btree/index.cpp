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

#include <boost/uuid/random_generator.hpp>

#include <algorithm>

namespace falcondb { namespace indexes { namespace btree {

static std::size_t ITEMS_PER_LEAF = 100; // completely arbitrary

index::index(interfaces::document_storage& storage, const document& definition, const document& root)
:   _storage(storage),
    _fields(definition.as_object().get_field("fields").as_object().to_map_of<int>()),
    _root(root)
{
}

index::index(interfaces::document_storage& storage, const document& definition)
:   _storage(storage),
    _fields(definition.as_object().get_field("fields").as_object().to_map_of<int>()),
    _root(generate_key())
{
    document root_doc = create_leaf();
    _storage.write(_root, root_doc);
}

index::~index()
{
}

void index::insert(const document& storage_key, const document& doc)
{
    document_list index_key = extract_index_key(doc);

    // enter the actual recursive algo
    tree_insert(_root, index_key, storage_key);

}

void index::update(const document& old_doc, const document& new_doc)
{
    // TODO
    assert(false);
}

void index::del(const document& doc)
{
    document_list index_key = extract_index_key(doc);

    tree_remove(_root, index_key);
}

document_list index::find(const document& range)
{
    // just return everything for now

    document_object root_doc = _storage.read(_root);
    const document_list& data = root_doc.get_field("data").as_list();

    document_list result;
    result.reserve(data.size());

    std::transform(
        data.begin(), data.end(),
        std::back_inserter(result),
        [](const document& in)
        {
            return in.as_object().get_field("value");
        });

    return result;
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

document index::generate_key()
{
    boost::uuids::random_generator gen;
    return document_scalar::from(gen());
}

document index::create_leaf()
{
    document_object leaf;
    leaf.insert(std::make_pair("type", document::from("leaf")));
    leaf.insert(std::make_pair("data", document_list()));

    return document(leaf);
}

void index::tree_insert(const document& node_key, const document_list& key, const document& value)
{
    document_object node = _storage.read(node_key);

    // insert into leaf node
    if(node.get_field("type").as_scalar().as<std::string>() == "leaf")
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
                    return compare_index_keys(
                        a.as_object().get_field("key").as_list(),
                        b.as_object().get_field("key").as_list());
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
    // insert into interior node
    else
    {
        // TODO implement
        assert(false);
    }
}

void index::tree_remove(const document& node_key, const document_list& key)
{
    document_object node = _storage.read(node_key);

    // remove from leaf node
    if(node.get_field("type").as_scalar().as<std::string>() == "leaf")
    {
        document_list& data = node.get_field("data").as_list();

        document_object search;
        search.insert(std::make_pair("key", key));

        auto range = std::equal_range(
            data.begin(), data.end(),
            search,
            [this](const document& a, const document& b)
            {
                return compare_index_keys(
                    a.as_object().get_field("key").as_list(),
                    b.as_object().get_field("key").as_list());
            });

        assert(range.first != range.second);
        data.erase(range.first, range.second);

        _storage.write(node_key, node);

        if (data.size() < ITEMS_PER_LEAF/2 )
        {
            // consolidate nodes or what?
        }
    }
    // remove from interior node
    else
    {
        // TODO implement
        assert(false);
    }
}

bool index::compare_index_keys(const document_list& a, const document_list& b) const
{
    // TODO ignore element sings now
    assert(a.size() == b.size());

    for(std::size_t i = 0; i< a.size(); ++i)
    {
        if (a[i] < b[i]) return true;
    }
    return false;
}

} } }
