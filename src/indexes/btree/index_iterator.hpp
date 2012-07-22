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

#ifndef FALCONDB_INDEXES_BTREE_INDEX_ITERATOR_HPP
#define FALCONDB_INDEXES_BTREE_INDEX_ITERATOR_HPP

#include "interfaces/index.hpp"

namespace falcondb { namespace indexes { namespace btree {

class index_iterator : public interfaces::index_iterator
{
public:
    index_iterator(const document& leaf_node, std::size_t index, interfaces::document_storage& storage);

    virtual bool has_next();

    virtual document next();

private:

    document _leaf_node;
    std::size_t _index;
    interfaces::document_storage& _storage;

};

} } }

#endif
