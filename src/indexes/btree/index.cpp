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

#include "indexes/btree/index.hpp"

#include <boost/uuid/random_generator.hpp>

namespace falcondb { namespace indexes { namespace btree {

index::index(interfaces::document_storage& storage, const document& definition, const document& root)
:   _storage(storage), _definition(definition), _root(root)
{
}

index::index(interfaces::document_storage& storage, const document& definition)
:    _storage(storage), _definition(definition), _root(generate_key())
{
    // TODO intialize root with empty page
}

void index::insert(const document& storage_key, const document& doc)
{
}

void index::update(const document& old_doc, const document& new_doc)
{
}

void index::del(const document& doc)
{
}

std::unique_ptr<interfaces::index_iterator> index::find(const document& range)
{
    return nullptr;
}

document index::generate_key()
{
    boost::uuids::random_generator gen;
    return document::from(gen());
}

} } }
