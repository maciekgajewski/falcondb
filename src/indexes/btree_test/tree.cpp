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

#include "indexes/btree/btree.hpp"

#include "interfaces/document_storage.hpp"

#include <boost/test/unit_test.hpp>

#include <memory>

namespace falcondb {

using indexes::btree::btree;

BOOST_AUTO_TEST_SUITE(btree_test_suite)

// fake storage
class test_document_storage : public interfaces::document_storage
{
public:

    virtual void write(const document& key, const document& doc)
    {
        _storage.erase(key);
        _storage.insert(std::make_pair(key, doc));
    }

    virtual document read(const document& key)
    {
        auto it = _storage.find(key);
        if (it == _storage.end())
        {
            throw exception("no such key in doc storage: ", key);
        }
        return it->second;
    }

    virtual void remove(const document& key)
    {
        _storage.erase(key);
    }

private:
    std::map<document, document> _storage;
};

// test env
class fixture
{
public:

    fixture()
    {
    }

    ~fixture()
    {
    }

    void init(bool unique)
    {
        _btree.reset(
            new btree(
                btree::create(_storage, document_scalar::from("root"), unique)));
    }

    template<typename T>
    static document_list key(const T& a) { return document_list({a}); }

    template<typename T, typename U>
    static document_list key(const T& a, const T& b) { return document_list({a, b}); }

    btree& get_tree() { return *_btree; }

private:

    std::unique_ptr<btree> _btree;
    test_document_storage _storage;
};

BOOST_FIXTURE_TEST_CASE(string_key_scan_non_unique, fixture)
{
    init(false);

    // TODO
}

BOOST_AUTO_TEST_SUITE_END()

} // ns
