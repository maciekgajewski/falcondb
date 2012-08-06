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

#include "backend_leveldb/database.hpp"

#include "utils/exception.hpp"

#include <cassert>

namespace falcondb { namespace backend_leveldb {

static inline leveldb::Slice to_slice(range& r) { return leveldb::Slice(r.begin(), r.size()); }
static inline range from_slice(const leveldb::Slice& s) { return range(s.data(), s.size()); }

database::database(leveldb::DB* db)
: _db(db)
{
    assert(db);
}

database::~database()
{
    // db closed automatically when destroyed
}

std::shared_ptr<database> database::open(const std::string& path)
{
    leveldb::Options options;
    options.create_if_missing = false;
    options.error_if_exists = false;

    leveldb::DB* db = nullptr;
    leveldb::Status s = leveldb::DB::Open(options, path, &db);
    throw_if_not_ok(s);

    return std::shared_ptr<database>(new database(db));
}

std::shared_ptr<database> database::create(const std::string& path)
{
    leveldb::Options options;
    options.error_if_exists = true;
    options.create_if_missing = true;

    leveldb::DB* db = nullptr;
    leveldb::Status s = leveldb::DB::Open(options, path, &db);
    throw_if_not_ok(s);

    return std::shared_ptr<database>(new database(db));
}

void database::drop()
{
    throw exception("backend_leveldb::database::drop not implemented");
}

void database::add(range key, range data)
{
    leveldb::WriteOptions options;
    leveldb::Status s = _db->Put(options, to_slice(key), to_slice(data));
    throw_if_not_ok(s);
}

void database::del(range key)
{
    leveldb::Status s = _db->Delete(leveldb::WriteOptions(), to_slice(key));
    throw_if_not_ok(s);
}

std::string database::get(range key)
{
    std::string out;
    leveldb::Status s = _db->Get(leveldb::ReadOptions(), to_slice(key), &out);
    throw_if_not_ok(s);

    return out;
}

void database::for_each(const interfaces::database_backend::key_value_handler& handler)
{
    std::unique_ptr<leveldb::Iterator> it(_db->NewIterator(leveldb::ReadOptions()));
    for (it->SeekToFirst(); it->Valid(); it->Next())
    {
        handler(from_slice(it->key()), from_slice(it->value()));
    }
    throw_if_not_ok(it->status());
}

void database::for_each(range begin, range end, const key_value_handler& handler)
{
    std::unique_ptr<leveldb::Iterator> it(_db->NewIterator(leveldb::ReadOptions()));
    std::string end_as_string = end.to_string();

    for (it->Seek(to_slice(begin));
        it->Valid() && it->key().ToString() < end_as_string;
        it->Next())
    {
        handler(from_slice(it->key()), from_slice(it->value()));
    }
    throw_if_not_ok(it->status());
}


void database::throw_if_not_ok(const leveldb::Status& status)
{
    if (!status.ok())
    {
        throw exception(status.ToString());
    }
}


} }
