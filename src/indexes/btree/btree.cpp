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

template<typename T>
std::string optional_to_string(const boost::optional<T>& opt)
{
    std::ostringstream ss;
    if (opt) ss << *opt;
    else ss << "none";
    return ss.str();
}

namespace detail
{
    // node summary
    struct node_summary
    {
        std::size_t count;
        document_list min_key;
        document_list max_key;
        document storage_key;
    };

    // insert result. ther leaf may have been split into two
    struct insert_result
    {
        node_summary left;
        boost::optional<node_summary> right;
    };

    // remove result
    struct remove_result
    {
        std::size_t removed_records;
        boost::optional<node_summary> node;
    };

}

btree btree::load(interfaces::document_storage& storage,  const document& storage_key, bool unique, std::size_t items_per_leaf)
{
    return btree(storage, storage_key, unique, items_per_leaf);
}

btree btree::create(interfaces::document_storage& storage,  const document& storage_key, bool unique, std::size_t items_per_leaf)
{
    document root_doc = create_leaf();
    document root_storage_key = generate_key();
    document_object meta;
    meta.set_field("root", root_storage_key);
    storage.write(root_storage_key, root_doc);
    storage.write(storage_key, meta);

    return btree(storage, storage_key, unique, items_per_leaf);
}

btree::btree(btree&& other)
:
    _storage(other._storage),
    _storage_key(std::move(other._root_storage_key)),
    _unique(other._unique),
    _items_per_leaf(other._items_per_leaf),

    _root_storage_key(document_scalar::null())
{
    load_meta_data();
}

btree::btree(interfaces::document_storage& storage, const document& storage_key, bool unique, std::size_t items_per_leaf)
:
    _storage(storage),
    _storage_key(storage_key),
    _unique(unique),
    _items_per_leaf(items_per_leaf),

    _root_storage_key(document_scalar::null())
{
    load_meta_data();
}

void btree::load_meta_data()
{
    document_object meta = _storage.read(_storage_key);
    _root_storage_key = meta.get_field("root");
}

void btree::store_meta_data()
{
    document_object meta;
    meta.set_field("root", _root_storage_key);
    _storage.write(_storage_key, meta);
}

document_list btree::scan(
    const boost::optional<document_list>& min,
    bool min_inclusive,
    const boost::optional<document_list>& max,
    bool max_inclusive,
    std::size_t limit,
    std::size_t skip)
{
//    std::cout << "===== tree node scan =======" << std::endl;
//    std::cout << "min: " << optional_to_string(min) << ", max: " << optional_to_string(max) << std::endl;
//    std::cout << "skip: " << skip << ", limit: " << limit << std::endl;
//    std::cout << std::endl;

    document_object node = _storage.read(_root_storage_key);
    return tree_scan(node, min, min_inclusive, max, max_inclusive, limit, skip);
}

document_list btree::tree_scan(
    const document_object& node,
    const boost::optional<document_list>& min,
    bool min_inclusive,
    const boost::optional<document_list>& max,
    bool max_inclusive,
    std::size_t limit,
    std::size_t skip)
{
    if (node.get_field("type").as_scalar().as<std::string>() == "leaf")
    {
        return tree_scan_leaf(node, min, min_inclusive, max, max_inclusive, limit, skip);
    }
    else
    {
        return tree_scan_interior(node, min, min_inclusive, max, max_inclusive, limit, skip);
    }
}

document_list btree::tree_scan_interior(
    const document_object& node,
    const boost::optional<document_list>& min,
    bool min_inclusive,
    const boost::optional<document_list>& max,
    bool max_inclusive,
    std::size_t limit,
    std::size_t skip)
{
    /*
    std::cout << "interior node scan:" << std::endl;
    std::cout << "min: " << optional_to_string(min) << ", max: " << optional_to_string(max) << std::endl;
    std::cout << "skip: " << skip << ", limit: " << limit << std::endl;
    std::cout << "node: " << node << std::endl;
    std::cout << std::endl;
    */

    const document_list& data = node.get_field("data");
    document_list::const_iterator it = data.begin();

    // boundary check - if max below colelction's min or min above max - return immediately
    if ( (min && *min > data.back().as_object().get_field("max").as_list())
        ||
        (max && *max < data.front().as_object().get_field("min").as_list()))
    {
        return document_list();
    }

    // if min is defined, look for node with min <= thatn the one
    if (min)
    {
        document_object search;
        search.set_field("min", *min);
        if (min_inclusive)
        {
            // needed: last element with min <= search.min
            // this is an equivalent of looking from the back for first with search.min > min
            auto rit = std::upper_bound(
                data.rbegin(), data.rend(),
                search,
                [] (const document& a, const document& b)
                {
                    return b.as_object().get_field("min") < a.as_object().get_field("min");
                });
            if (rit != data.rend())
                std::advance(it, std::distance(rit, data.rend()) - 1);
        }
        else
        {
            auto rit = std::lower_bound(
                data.rbegin(), data.rend(),
                search,
                [] (const document& a, const document& b)
                {
                    return b.as_object().get_field("min") < a.as_object().get_field("min");
                });
            if (rit != data.rend())
                std::advance(it, std::distance(rit, data.rend()) - 1);
        }
    }

    // skip entire tree branches
    if (skip > 0)
    {
        while(true)
        {
            std::size_t count = it->as_object().get_field("count").as_scalar().as<std::size_t>();
            if(skip >= count && it != data.end())
            {
                skip -= count;
                ++it;
            }
            else
                break;
        }
        if (it == data.end())
        {
            return document_list();
        }
    }

    document inferior_node_storage_key = it->as_object().get_field("storage");
    document inferior_node = _storage.read(inferior_node_storage_key);

    return tree_scan(inferior_node, min, min_inclusive, max, max_inclusive, limit, skip);
}

document_list btree::tree_scan_leaf(
    const document_object& nodep,
    const boost::optional<document_list>& min,
    bool min_inclusive,
    const boost::optional<document_list>& max,
    bool max_inclusive,
    std::size_t limit,
    std::size_t skip)
{
    document_list result;
    document_object node = nodep; // local copy for iteration
    while(true)
    {
        /*
        std::cout << "leaf node scan:" << std::endl;
        std::cout << "min: " << optional_to_string(min) << " inclusive: " << std::boolalpha << min_inclusive << std::endl;
        std::cout << "max: " << optional_to_string(max) << " inlcusive: " << std::boolalpha << max_inclusive << std::endl;
        std::cout << "skip: " << skip << ", limit: " << limit << std::endl;
        std::cout << "node: " << node << std::endl;
        std::cout << std::endl;
        */

        const document_list& data = node.get_field("data");

        // full scan of the array. this could be asyli optimized
        for(auto kv : data)
        {
            document_list key = kv.as_object().get_field("key");
            document val = kv.as_object().get_field("value");

            // skip below min
            if (min && (key < *min || (!min_inclusive && key == *min)))
            {
                continue;
            }

            // skip & limit
            if (skip > 0)
            {
                skip--;
                continue;
            }
            if(limit-- == 0)
            {
                return result;
            }

            // return if max reached
            if (max && ((*max < key) || (!max_inclusive && key == *max)))
            {
                return result;
            }

            result.push_back(val);
        }

        document next_node_storage_key = node.get_field("next");
        if (!next_node_storage_key.is_null())
        {
            node = _storage.read(next_node_storage_key);
        }
        else
        {
            return result;
        }
    }
}

void btree::insert(const document_list key, const document& value)
{
    detail::insert_result result = tree_insert(_root_storage_key, key, value);

    // do we need new root?
    if (result.right)
    {
        document_object new_root = create_interior(result);
        _root_storage_key = generate_key();
        _storage.write(_root_storage_key, new_root);

        store_meta_data();
    }
}

document_object btree::create_interior()
{
    document_object node;
    node.set_field("type", document::from("interior"));
    node.set_field("data", document_list());

    return node;
}

document_object btree::create_interior(const detail::insert_result& insert_result)
{
    assert(insert_result.right);

    document_object left;
    left.set_field("min", document::from(insert_result.left.min_key));
    left.set_field("max", document::from(insert_result.left.max_key));
    left.set_field("count", document::from(insert_result.left.count));
    left.set_field("storage", document::from(insert_result.left.storage_key));

    document_object right;
    right.set_field("min", document::from(insert_result.right->min_key));
    right.set_field("max", document::from(insert_result.right->max_key));
    right.set_field("count", document::from(insert_result.right->count));
    right.set_field("storage", document::from(insert_result.right->storage_key));

    document_list entries {left, right};

    document_object node;
    node.set_field("type", document::from("interior"));
    node.set_field("data", entries);

    return node;
}


std::size_t btree::remove(const document_list& key)
{
    detail::remove_result result = tree_remove(_root_storage_key, key);
    if (result.removed_records > 0 && !result.node)
    {
        // the root has been removed, the index is now empty. reinitialize root
        document_object new_root = create_leaf();
        _storage.write(_root_storage_key, new_root);
    }
    return result.removed_records;
}

document btree::generate_key()
{
    boost::uuids::random_generator gen;
    return document_scalar::from(gen());
}

document_object btree::create_leaf()
{
    document_object leaf;
    leaf.insert(std::make_pair("type", document::from("leaf")));
    leaf.insert(std::make_pair("data", document_list()));
    leaf.insert(std::make_pair("prev", document_scalar::null()));
    leaf.insert(std::make_pair("next", document_scalar::null()));

    return document(leaf);
}

detail::insert_result btree::tree_insert(const document& node_key, const document_list& key, const document& value)
{
    document_object node = _storage.read(node_key);

    if(node.get_field("type").as_scalar().as<std::string>() == "leaf")
    {
        return tree_insert_leaf(node_key, node, key, value);
    }
    else
    {
        return tree_insert_interior(node_key, node, key, value);
    }
}

detail::insert_result btree::tree_insert_leaf(
    const document& node_key,
    document_object& node,
    const document_list& key,
    const document& value)
{
    document_list& data = node.get_field("data").as_list();
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

    // will fit?
    if (data.size() <= _items_per_leaf)
    {
        _storage.write(node_key, node);
        return detail::insert_result {
            detail::node_summary {
                data.size(),
                data.front().as_object().get_field("key"),
                data.back().as_object().get_field("key"),
                node_key },
            boost::none };
    }
    else
    {
        // split data, move upper half to new node
        const std::size_t lh = _items_per_leaf / 2;
        const std::size_t uh = _items_per_leaf - lh;

        document_list upper_half;
        upper_half.reserve(uh);
        std::copy(data.begin() + lh, data.end(), std::back_inserter(upper_half));
        document_object new_leaf = create_leaf();
        new_leaf.set_field("prev", node_key);
        new_leaf.set_field("next", node.get_field("next"));
        new_leaf.set_field("data", upper_half);
        document new_leafs_storage_key = generate_key();
        _storage.write(new_leafs_storage_key, new_leaf);

        // truncate data in existing node
        document_list lower_half;
        lower_half.reserve(lh);
        std::copy(data.begin(), data.begin() + lh, std::back_inserter(lower_half));
        data.swap(lower_half);
        node.set_field("next", new_leafs_storage_key);
        _storage.write(node_key, node);

        return detail::insert_result {
            detail::node_summary {
                data.size(),
                data.front().as_object().get_field("key"),
                data.back().as_object().get_field("key"),
                node_key },
            detail::node_summary {
                upper_half.size(),
                upper_half.front().as_object().get_field("key"),
                upper_half.back().as_object().get_field("key"),
                new_leafs_storage_key }};
    }
}

static std::size_t sum_count(const document_list& data)
{
    std::size_t count = 0;
    for(auto n : data)
    {
        count += n.as_object().get_field("count").as_scalar().as<std::size_t>();
    }
    return count;
}

detail::insert_result btree::tree_insert_interior(
    const document& node_key,
    document_object& node,
    const document_list& key,
    const document& value)
{
    document_list& data = node.get_field("data").as_list();

    // find the node where min >= key
    document_object search;
    search.set_field("min", key);
    auto it = std::lower_bound(
        data.begin(), data.end(),
        search,
        [] (const document& a, const document& b)
        {
            return a.as_object().get_field("min") < b.as_object().get_field("min");
        });

    if (it == data.end())
    {
        it = data.end() - 1; // use the last element
    }

    document inferior_node_key = it->as_object().get_field("storage");
    detail::insert_result result = tree_insert(inferior_node_key, key, value);

    // update the entry
    document_object left;
    left.set_field("storage", result.left.storage_key);
    left.set_field("min", result.left.min_key);
    left.set_field("max", result.left.max_key);
    left.set_field("count", document_scalar::from(result.left.count));

    // if the insert caused the element to split, insert new node here
    if (result.right)
    {
        document_object right;
        right.set_field("storage", result.right->storage_key);
        right.set_field("min", result.right->min_key);
        right.set_field("max", result.right->max_key);
        right.set_field("count",document_scalar::from( result.right->count));

        *it = right;
        data.insert(it, left); // ordering is importaint here! nisert may invalidate the container
    }
    else
    {
        *it = left;
    }
    _storage.write(node_key, node);

    // if the node is too big - split it
    if (data.size() > _items_per_leaf)
    {
        // split data, move upper half to new node
        const std::size_t lh = _items_per_leaf / 2;
        const std::size_t uh = _items_per_leaf - lh;

        document_list upper_half;
        upper_half.reserve(uh);
        std::copy(data.begin() + lh, data.end(), std::back_inserter(upper_half));
        document_object new_interior = create_interior();
        new_interior.set_field("data", upper_half);
        document new_interior_storage_key = generate_key();
        _storage.write(new_interior_storage_key, new_interior);

        // truncate data in existing node
        document_list lower_half;
        lower_half.reserve(lh);
        std::copy(data.begin(), data.begin() + lh, std::back_inserter(lower_half));
        data.swap(lower_half);
        _storage.write(node_key, node);

        return detail::insert_result {
            detail::node_summary {
                sum_count(data),
                data.front().as_object().get_field("min"),
                data.back().as_object().get_field("max"),
                node_key },
            detail::node_summary {
                sum_count(upper_half),
                upper_half.front().as_object().get_field("min"),
                upper_half.back().as_object().get_field("max"),
                new_interior_storage_key }};
    }
    else
    {
        // count elements

        return detail::insert_result {
            detail::node_summary {
                sum_count(data),
                data.front().as_object().get_field("min"),
                data.back().as_object().get_field("max"),
                node_key },
            boost::none };
    }
}

detail::remove_result btree::tree_remove(const document& node_storage_key, const document_list& key)
{
    document_object node = _storage.read(node_storage_key);

    if(node.get_field("type").as_scalar().as<std::string>() == "leaf")
    {
        return tree_remove_leaf(node_storage_key, node, key);
    }
    else
    {
        return tree_remove_interior(node_storage_key, node, key);
    }
}

detail::remove_result btree::tree_remove_leaf(
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

    std::size_t removed_items = std::distance(range.first, range.second);
    data.erase(range.first, range.second);

    if (removed_items > 0)
    {
        if (data.empty())
        {
            _storage.remove(node_storage_key);
            return detail::remove_result { removed_items, boost::none };
        }
        else
        {
            _storage.write(node_storage_key, node);
        }
    }

    return detail::remove_result {
        removed_items,
        detail::node_summary {
            data.size(),
            data.front().as_object().get_field("key"),
            data.back().as_object().get_field("key"),
            node_storage_key }
        };
}

static document_list::iterator find_inferiror_for_key(document_list& data, const document_list& key)
{
    for(auto it = data.begin(); it != data.end(); ++it)
    {
        const document_list& min = it->as_object().get_field("min").as_list();
        const document_list& max = it->as_object().get_field("max").as_list();
        if (min <= key && key <= max)
            return it;
    }
    return data.end();
}

detail::remove_result btree::tree_remove_interior(
    const document& node_storage_key,
    document_object& node,
    const document_list& key)
{
    document_list& data = node.get_field("data").as_list();

    // find the node where min >= key
    auto it = find_inferiror_for_key(data, key);

    if (it == data.end())
    {
        return detail::remove_result { 0, boost::none };
    }

    document_object& inferior_summary = it->as_object();
    document inferior_node_key = inferior_summary.get_field("storage");
    detail::remove_result result = tree_remove(inferior_node_key, key);

    // handle the removal
    if (result.removed_records == 0)
    {
        // nothing removed
        return result;
    }
    else if (result.node)
    {
        // something removed, but the node is still exists
        inferior_summary.set_field("min", result.node->min_key);
        inferior_summary.set_field("max", result.node->max_key);
        inferior_summary.set_field("count", document_scalar::from(result.node->count));

        _storage.write(node_storage_key, node);

        return detail::remove_result {
            result.removed_records,
            detail::node_summary {
                sum_count(data),
                data.front().as_object().get_field("min"),
                data.back().as_object().get_field("max"),
                node_storage_key }
            };
    }
    else
    {
        // the inferior node has been removed
        data.erase(it);

        // shall we remove ourselves?
        if (data.empty())
        {
            _storage.remove(node_storage_key);
            return detail::remove_result { result.removed_records, boost::none };
        }
        else
        {
            // something removed, but the node is still exists
            inferior_summary.set_field("min", result.node->min_key);
            inferior_summary.set_field("max", result.node->max_key);
            inferior_summary.set_field("count", document_scalar::from(result.node->count));

            _storage.write(node_storage_key, node);

            return detail::remove_result {
                result.removed_records,
                detail::node_summary {
                    sum_count(data),
                    data.front().as_object().get_field("min"),
                    data.back().as_object().get_field("max"),
                    node_storage_key }
                };
        }
    }
}

}}} // ns
