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

#include "dbengine/document_storage.hpp"

namespace falcondb { namespace dbengine {

document_storage::document_storage(const interfaces::database_backend_ptr& raw_storage, const std::string& ns)
    : _raw_storage(raw_storage), _ns(ns)
{
}

void document_storage::write(const falcondb::document& key, const falcondb::document& doc)
{
    std::string key_data = _ns + key.to_json();
    std::string doc_data = doc.to_json();

    _raw_storage->add(key_data, doc_data);
}

document document_storage::read(const document& key)
{
    std::string key_data = _ns + key.to_json();
    try
    {
        std::string doc_data = _raw_storage->get(key_data);
        return document::from_json(doc_data);
    }
    catch(const std::exception& e)
    {
        // add more useful info to the exception
        throw exception("Error reading key ", key_data, " from storage: ", e.what());
    }
}

void document_storage::del(const document& key)
{
    std::string key_data = _ns + key.to_json();
    _raw_storage->del(key_data);
}

} }
