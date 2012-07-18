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

#ifndef FALCONDB_INDEX_BTREE_INDEX_HPP
#define FALCONDB_INDEX_BTREE_INDEX_HPP

#include "interfaces/index.hpp"

namespace falcondb { namespace index_btree {

class index : public interfaces::index
{
public:
    index();

    virtual void insert(const document& storage_key, const document& doc);

    virtual void update(const document& old_doc, const document& new_doc);

    virtual void del(const document& doc);

    virtual std::unique_ptr<interfaces::index_iterator> find(const document& range);
};

} }

#endif
