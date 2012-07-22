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

class document_storage;

/// Iterator returned by index
class index_iterator
{
public:

    typedef std::unique_ptr<index_iterator> unique_ptr;

    /// Checks if subsequent call to 'next' will succeed
    virtual bool has_next() = 0;

    /// Returns document storage key for the next document, moves the iterator
    virtual document next() = 0;
};

/// Index abstraction
class index
{
public:

    typedef std::shared_ptr<index> shared_ptr;
    typedef std::unique_ptr<index> unique_ptr;

    virtual ~index() {};

    /// Inserts document into index
    virtual void insert(const document& storage_key, const document& doc) = 0;

    /// Updates index when document is changed
    virtual void update(const document& old_doc, const document& new_doc) = 0;

    /// Removes document from index
    virtual void del(const document& doc) = 0;

    /// Creates iterator on certain range.
    virtual index_iterator::unique_ptr find(const document& range) = 0;
};

/// Index type
class index_type
{
public:

    typedef std::shared_ptr<index_type> pointer;

    struct create_result
    {
        document index_description;
        std::unique_ptr<index> new_index;
    };

    /// Loads index from storage, which has it's root stored under specific key
    virtual std::unique_ptr<index> load_index(
        document_storage& index_storage,
        const document& index_description) = 0;

    // Creates index on all elements from the iterator.
    // The index is stored in the index storage. The description has to be stored
    // and is required to load the index
    virtual create_result create_index(
        const document& index_definition,
        index_iterator& documents,
        document_storage& index_storage,
        document_storage& data_storage) = 0;
};

} } // namespaces

#endif
