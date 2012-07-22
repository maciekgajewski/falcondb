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
:   _storage(storage), _definition(definition), _root(root)
{
}

index::index(interfaces::document_storage& storage, const document& definition)
:    _storage(storage), _definition(definition), _root(generate_key())
{
    document root_doc = create_leaf();
    _storage.write(_root, root_doc);
}

index::~index()
{
}

void index::insert(const document& storage_key, const document& doc)
{
    document index_key = extract_index_key(doc);

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
    document index_key = extract_index_key(doc);

    tree_remove(_root, index_key);
}

std::unique_ptr<interfaces::index_iterator> index::find(const document& range)
{
    document root_doc = _storage.read(_root);
    return std::unique_ptr<interfaces::index_iterator>(new index_iterator(root_doc, 0, _storage));
}

document index::extract_index_key(const document& doc)
{
    document fields = _definition.get<document>("fields");
    std::vector<std::string> field_names = fields.field_names();

    std::vector<document> index_key;
    index_key.reserve(field_names.size());

    std::transform(field_names.begin(), field_names.end(), std::back_inserter(index_key),
        [&doc](const std::string& dotted_name){ return doc.get_dotted<document>(dotted_name); });

    return document::from_array(index_key);
}

document index::generate_key()
{
    boost::uuids::random_generator gen;
    return document::from(gen());
}

document index::create_leaf()
{
    document leaf;
    leaf.append("type", "leaf");
    leaf.append("data", document::empty_array());

    return leaf;
}

void index::tree_insert(const document& node_key, const document& key, const document& value)
{
    document node = _storage.read(node_key);

    // insert into leaf node
    if(node.get<std::string>("type") == "leaf")
    {
        document data = node.get<document>("data");
        // will fit?
        if (data.size() < ITEMS_PER_LEAF)
        {
            document new_element;
            new_element.append("key", key);
            new_element.append("value", value);

            document::const_iterator it = std::lower_bound(
                data.begin(), data.end(),
                new_element,
                [this](const document& a, const document& b)
                {
                    return compare_index_keys(a.get<document>("key"), b.get<document>("key"));
                });

            data.insert(it, new_element);

            node.append("data", data); // TODO there should be an ability to do in-place update
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

void index::tree_remove(const document& node_key, const document& key)
{
    document node = _storage.read(node_key);

    // remove from leaf node
    if(node.get<std::string>("type") == "leaf")
    {
        document data = node.get<document>("data");

        document search;
        search.append("key", key);

        auto range = std::equal_range(
            data.begin(), data.end(),
            search,
            [this](const document& a, const document& b)
            {
                return compare_index_keys(a.get<document>("key"), b.get<document>("key"));
            });

        assert(range.first != range.second);
        data.erase(range.first, range.second);

        node.append("data", data); // TODO there should be an ability to do in-place update
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

bool index::compare_index_keys(const document& a, const document& b) const
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
