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
#include "indexes/btree/index_iterator.hpp"

#include "interfaces/document_storage.hpp"

#include <boost/uuid/random_generator.hpp>

#include <algorithm>

namespace falcondb { namespace indexes { namespace btree {

static std::size_t ITEMS_PER_LEAF = 100; // completely arbitrary

index::index(interfaces::document_storage& storage, const document& definition, const document& root)
:   _storage(storage),
    _fields(definition.get_field_as<document>("fields").as_map_of<int>()),
    _root(root)
{
}

index::index(interfaces::document_storage& storage, const document& definition)
:   _storage(storage),
    _fields(definition.get_field_as<document>("fields").as_map_of<int>()),
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
    document_array index_key = extract_index_key(doc);

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
    document_array index_key = extract_index_key(doc);

    tree_remove(_root, index_key);
}

std::unique_ptr<interfaces::index_iterator> index::find(const document& range)
{
    document root_doc = _storage.read(_root);
    return std::unique_ptr<interfaces::index_iterator>(new index_iterator(root_doc, 0, _storage));
}

document_array index::extract_index_key(const document& doc)
{
    const document_map& as_map = doc.as_map();
    document_array result;
    result.reserve(_fields.size());

    for(auto field : _fields)
    {
        auto it = as_map.find(field.first);
        if (it == as_map.end())
        {
            // TODO insert null
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
    return document(gen());
}

document index::create_leaf()
{
    document_map leaf;
    leaf.insert(std::make_pair("type", "leaf"));
    leaf.insert(std::make_pair("data", document_array()));

    return document(leaf);
}

void index::tree_insert(const document& node_key, const document_array& key, const document& value)
{
    document node = _storage.read(node_key);

    // insert into leaf node
    if(node.get_field_as<std::string>("type") == "leaf")
    {
        document_array data = node.get_field_as_array("data");
        // will fit?
        if (data.size() < ITEMS_PER_LEAF)
        {
            document_map new_element;
            new_element.insert(std::make_pair("key", key));
            new_element.insert(std::make_pair("value", value.as_any()));

            auto it = std::lower_bound(
                data.begin(), data.end(),
                new_element,
                [this](const document_any& a, const document_any& b)
                {
                    return compare_index_keys(
                        document(a).get_field_as_array("key"),
                        document(b).get_field_as_array("key"));
                });

            data.insert(it, document_any(new_element));

            node.as_map().find("data")->second =  data;
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

void index::tree_remove(const document& node_key, const document_array& key)
{
    document node = _storage.read(node_key);

    // remove from leaf node
    if(node.get_field_as<std::string>("type") == "leaf")
    {
        document_array data = node.get_field_as_array("data");

        document_map search;
        search.insert(std::make_pair("key", key));

        auto range = std::equal_range(
            data.begin(), data.end(),
            search,
            [this](const document_any& a, const document_any& b)
            {
                return compare_index_keys(
                    document(a).get_field_as_array("key"),
                    document(b).get_field_as_array("key"));
            });

        assert(range.first != range.second);
        data.erase(range.first, range.second);

        node.as_map().find("data")->second = data;
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

bool index::compare_index_keys(const document_array& a, const document_array& b) const
{
    // TODO ignore element sings now
    assert(a.size() == b.size());

    for(std::size_t i = 0; i< a.size(); ++i)
    {
        if (document_wrapper(a[i]) < document_wrapper(b[i])) return true;
    }
    return false;
}

} } }
