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

#ifndef FALCONDB_INTERFACES_DOCUMENT_STORAGE_HPP
#define FALCONDB_INTERFACES_DOCUMENT_STORAGE_HPP

#include "document/document.hpp"

namespace falcondb { namespace interfaces {


/// Serialized storage. Allows for storing and retrieving documents.
/// Keys used by a store must be uniqe per storage instance; they don't have to be globally unique
class document_storage
{
public:

    /// iteration callback
    typedef std::function<void (const document& key, const document& value)> key_value_handler;

    /// Stores document
    virtual void write(const document& key, const document& doc) = 0;

    /// Retrieves document
    virtual document read(const document& key) = 0;

    /// Deletes document
    virtual void remove(const document& key) = 0;

    /// Iterates over all documents
    virtual void for_each(const key_value_handler& fun) = 0;

};

}} // namespaces

#endif
