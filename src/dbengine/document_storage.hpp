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

#ifndef FALCONDB_DBENGINE_DOCUMENT_STORAGE_HPP
#define FALCONDB_DBENGINE_DOCUMENT_STORAGE_HPP

#include "interfaces/document_storage.hpp"
#include "interfaces/storage_backend.hpp"

namespace falcondb { namespace dbengine {

/// Simple implementation of serializing storage.
/// Adds namespace prefix to each key after serialization
class document_storage : public interfaces::document_storage
{
public:
    document_storage(const interfaces::database_backend_ptr& raw_storage, const std::string& ns);

    // document_storage

    virtual void write(const document& key, const document& doc);
    virtual document read(const document& key);
    virtual void del(const document& key);

private:

    interfaces::database_backend_ptr _raw_storage;
    const std::string _ns;
};


} } // ns

#endif
