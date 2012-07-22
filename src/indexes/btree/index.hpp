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

#ifndef FALCONDB_INDEXES_BTREE_INDEX_HPP
#define FALCONDB_INDEXES_BTREE_INDEX_HPP

#include "interfaces/index.hpp"

namespace falcondb { namespace indexes { namespace btree {

class index : public interfaces::index
{
public:

    // creates index, loads content
    index(
        interfaces::document_storage& storage,
        const document& definition,
        const document& root);

    // creates empty index
    index(
        interfaces::document_storage& storage,
        const document& definition);

    virtual ~index();

    document get_root() const { return _root; }

    // interface

    virtual void insert(const document& storage_key, const document& doc);

    virtual void update(const document& old_doc, const document& new_doc);

    virtual void del(const document& doc);

    virtual std::unique_ptr<interfaces::index_iterator> find(const document& range);

private:

    /// Reducec document to an array containing values related to fields specified in index definition
    document extract_index_key(const document& doc);
    bool compare_index_keys(const document& a, const document& b) const;

    static document generate_key();
    static document create_leaf();

    // tree ops

    void tree_insert(const document& node_key, const document& key, const document& value);
    void tree_remove(const document& node_key, const document& key);

    interfaces::document_storage& _storage;
    const document _definition;
    const document _root;

};

} } }

#endif
