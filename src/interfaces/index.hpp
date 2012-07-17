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

#ifndef FALCONDB_INTERFACES_INDEX_HPP
#define FALCONDB_INTERFACES_INDEX_HPP

#include "document/document.hpp"

#include <memory>

namespace falcondb { namespace interfaces {

/// Iterator returned by index
class index_iterator
{
public:

    /// Checks if subsequent call to 'next' will succeed
    virtual bool has_next() = 0;

    /// Returns document storage key for the next document
    virtual document next() = 0;
};

/// Index abstraction
class index
{
public:

    /// Inserts document into index
    virtual void insert(const document& storage_key, const document& doc) = 0;

    /// Updates index when document is changed
    virtual void update(const document& old_doc, const document& new_doc) = 0;

    /// Removes document from index
    virtual void del(const document& doc) = 0;

    /// ????
    virtual std::unique_ptr<index_iterator> find(const document& e) = 0;
};

} } // namespaces

#endif
