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

#ifndef FALCONDB_INDEXES_BTREE_INDEX_TYPE_HPP
#define FALCONDB_INDEXES_BTREE_INDEX_TYPE_HPP

#include "interfaces/index.hpp"

namespace falcondb { namespace indexes { namespace btree {

/// Implments 'index type' for btree indexes
class index_type : public interfaces::index_type
{
public:
    index_type();

    // interface

    virtual std::unique_ptr<interfaces::index> load_index(
        interfaces::document_storage& index_storage,
        const document& index_description);

    virtual create_result create_index(const document& index_definition,
        interfaces::document_storage& index_storage,
        interfaces::document_storage& data_storage);

    // other

    /// Throws if defintio is invalid
    static void verify_definition(const document& definition);
};

} } }

#endif
