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

#ifndef FALCONDB_INDEXES_BTREE_HPP
#define FALCONDB_INDEXES_BTREE_HPP

#include "interfaces/document_storage.hpp"

#include <boost/optional.hpp>

#include <limits>

namespace falcondb { namespace indexes { namespace btree {

namespace detail
{
    class insert_result;
}

/// B-tree stores key - value pairs in a storage-backend tree
/// Key has to be a document_list, value can be anything
class btree
{
public:

    // constructors
    static btree load(
        interfaces::document_storage& storage,
        const document& storage_key,
        bool unique,
        std::size_t items_per_leaf);

    static btree create(
        interfaces::document_storage& storage,
        const document& storage_key,
        bool unique,
        std::size_t items_per_leaf);

    btree(btree&& other);

    // read data from tree
    document_list scan(
        const boost::optional<document_list>& min,
        bool min_inclusive,
        const boost::optional<document_list>& max,
        bool max_inclusive,
        std::size_t limit = std::numeric_limits<std::size_t>::max(),
        std::size_t skip = 0);

    void insert(const document_list key, const document& value);
    void remove(const document_list& key);

private:

    btree(
        interfaces::document_storage& storage,
        const document& root_storage_key,
        bool unique,
        std::size_t items_per_leaf);


    // insert

    detail::insert_result tree_insert(const document& node_key, const document_list& key, const document& value);

    detail::insert_result tree_insert_leaf(
        const document& node_key,
        document_object& node,
        const document_list& key,
        const document& value);

    detail::insert_result tree_insert_interior(
        const document& node_key,
        document_object& node,
        const document_list& key,
        const document& value);

    // remove

    void tree_remove(const document& node_key, const document_list& key);

    void tree_remove_leaf(
        const document& node_key,
        document_object& node,
        const document_list& key);

    void tree_remove_interior(
        const document& node_key,
        document_object& node,
        const document_list& key);


    // scan

    document_list tree_scan(
        const document_object& node,
        const boost::optional<document_list>& min,
        bool min_inclusive,
        const boost::optional<document_list>& max,
        bool max_inclusive,
        std::size_t limit,
        std::size_t skip);

    document_list tree_scan_leaf(
        const document_object& node,
        const boost::optional<document_list>& min,
        bool min_inclusive,
        const boost::optional<document_list>& max,
        bool max_inclusive,
        std::size_t limit,
        std::size_t skip);

    document_list tree_scan_interior(
        const document_object& node,
        const boost::optional<document_list>& min,
        bool min_inclusive,
        const boost::optional<document_list>& max,
        bool max_inclusive,
        std::size_t limit,
        std::size_t skip);









    static document generate_key();
    static document_object create_leaf();
    static document_object create_interior(const detail::insert_result& insert_result);
    static document_object create_interior();

    void load_meta_data();
    void store_meta_data();

    interfaces::document_storage& _storage;
    const document _storage_key;
    const bool _unique;
    const std::size_t _items_per_leaf;

    document _root_storage_key;
};

}}} // ns

#endif
