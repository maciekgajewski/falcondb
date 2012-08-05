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

#include "document/json_writer.hpp"

#include <boost/test/unit_test.hpp>

#include <memory>
#include <sstream>
#include <iomanip>

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

    void dump()
    {
        for(auto p : _storage)
        {
            std::cout << p.first << "  =>  " << p.second << std::endl;
        }
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
                btree::create(_storage, document_scalar::from(std::string("main")), unique, 4)));
    }

    template<typename T>
    static document_list make_key(const T& a) { return document_list({document::from(a)}); }

    template<typename T, typename U>
    static document_list make_key(const T& a, const T& b) { return document_list({document::from(a), document::from(b)}); }

    btree& get_tree() { return *_btree; }

    void dump_storage()
    {
        _storage.dump();
        std::cout << std::endl << std::endl;
    }

    bool exists(const document_list& key)
    {
        document_list result = _btree->scan(key, true, key, true);
        return !result.empty();
    }

private:

    std::unique_ptr<btree> _btree;
    test_document_storage _storage;
};

BOOST_FIXTURE_TEST_CASE(string_key_scan_non_unique, fixture)
{
    init(false);

    // populate with keys in range 1000-2999
    unsigned to_insert = 2000;
    unsigned base = 1000;
    for(unsigned i = 0; i < to_insert; ++i)
    {
        std::ostringstream ss;
        ss << std::setw(4) << (i+base);
        document_list key = make_key(ss.str());

        document_scalar value = document_scalar::from(i+base);

        //BEGIN debug
        //dump_storage(); // to debug some nasty issues
        //std::cout << "inserting " << key << " -> " << value << std::endl;
        //END debug

        get_tree().insert(key, value);
    }
    // search below
    {
        document_list result = get_tree().scan(
            make_key("0000"), true,
            make_key("1000"), false);
        BOOST_CHECK_EQUAL(result.size(), 0);
    }

    // search above
    {
        document_list result = get_tree().scan(
            make_key("2999"), false,
            make_key("5678"), true);
        BOOST_CHECK_EQUAL(result.size(), 0);
    }

    // search below including the first one
    {
        document_list result = get_tree().scan(
            make_key("0000"), false,
            make_key("1000"), true);
        BOOST_REQUIRE_EQUAL(result.size(), 1);
        BOOST_CHECK_EQUAL(result[0].as_scalar(), document_scalar::from(1000));
    }

    // serch inside the range
    {
        document_list result = get_tree().scan(
            make_key("1500"), false,
            make_key("1600"), true);
        BOOST_REQUIRE_EQUAL(result.size(), 100);
        for(int i = 0; i < 100; ++i)
        {
            BOOST_CHECK_EQUAL(result[i].as_scalar(), document_scalar::from(1501+i));
        }
    }
}

BOOST_FIXTURE_TEST_CASE(int_key_removal, fixture)
{
    init(false);

    for (int i=0; i < 20; i++)
    {
        get_tree().insert(make_key(i), document_scalar::from(i));
    }

    BOOST_CHECK_EQUAL(exists(make_key(0)), true);
    BOOST_CHECK_EQUAL(exists(make_key(5)), true);
    BOOST_CHECK_EQUAL(exists(make_key(15)), true);
    BOOST_CHECK_EQUAL(exists(make_key(19)), true);
    BOOST_CHECK_EQUAL(exists(make_key(20)), false);

    {
        std::size_t removed = get_tree().remove(make_key(22));
        BOOST_CHECK_EQUAL(removed, 0);
    }

    {
        std::size_t removed = get_tree().remove(make_key(15));
        BOOST_CHECK_EQUAL(removed, 1);
        BOOST_CHECK_EQUAL(exists(make_key(15)), false);
    }

    {
        std::size_t removed = get_tree().remove(make_key(5));
        BOOST_CHECK_EQUAL(removed, 1);
        BOOST_CHECK_EQUAL(exists(make_key(5)), false);
    }

    {
        std::size_t removed = get_tree().remove(make_key(0));
        BOOST_CHECK_EQUAL(removed, 1);
        BOOST_CHECK_EQUAL(exists(make_key(0)), false);
    }

    {
        std::size_t removed = get_tree().remove(make_key(19));
        BOOST_CHECK_EQUAL(removed, 1);
        BOOST_CHECK_EQUAL(exists(make_key(19)), false);
    }
}

BOOST_AUTO_TEST_SUITE_END()

} // ns
