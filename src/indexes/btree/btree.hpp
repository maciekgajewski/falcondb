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

namespace falcondb { namespace indexes { namespace btree {

/// B-tree stores key - value pairs in a storage-backend tree
/// Key has to be a document_list, value can be anything
class btree
{
public:

    // constructors
    static btree load(interfaces::document_storage& storage,  const document& root_storage_key);
    static btree create(interfaces::document_storage& storage,  const document& root_storage_key);

    // read data from tree
    document_list scan(
        const boost::optional<document_list>& low,
        const boost::optional<document_list>& hi,
        std::size_t limit,
        std::size_t skip);

    void insert(const document_list key, const document& value);
    void remove(const document_list& key);

private:

    btree(interfaces::document_storage& storage,  const document& root_storage_key);


    void tree_insert(const document& node_key, const document_list& key, const document& value);

    void tree_insert_leaf(
        const document& node_key,
        document_object& node,
        const document_list& key,
        const document& value);

    void tree_insert_interior(
        const document& node_key,
        document_object& node,
        const document_list& key,
        const document& value);

    void tree_remove(const document& node_key, const document_list& key);

    void tree_remove_leaf(
        const document& node_key,
        document_object& node,
        const document_list& key);

    void tree_remove_interior(
        const document& node_key,
        document_object& node,
        const document_list& key);

    static document generate_key();
    static document create_leaf();

    interfaces::document_storage& _storage;
    const document _root_storage_key;

};

}}} // ns

#endif
