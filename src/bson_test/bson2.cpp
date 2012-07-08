/*    Original test case content: Copyright 2009 10gen Inc.
 *    Adaptation fot boost test framework Copyright (C) 2012 Maciej Gajewski <maciej.gajewski0 at gmail dot com>
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

// tests below copied/adaptde from mongos test suite

#include <boost/test/unit_test.hpp>

#include "bson/bson.hpp"

using namespace mongo;

BOOST_AUTO_TEST_SUITE(bson_test_suite2)

class Base
{
protected:
    template< typename T >
    static BSONObj basic( const char *name, const T& val ) {
        BSONObjBuilder b;
        b.append( name, val );
        return b.obj();
    }
    template< typename T, typename U >
    static BSONObj basic( const char *name, const T& val, const char* name2, const U& val2 ) {
        BSONObjBuilder b;
        b.append( name, val );
        b.append( name2, val2 );
        return b.obj();
    }
    template< typename T, typename U, typename V >
    static BSONObj basic( const char *n1, const T& v1, const char* n2, const U& v2, const char* n3, const V& v3) {
        BSONObjBuilder b;
        b.append( n1, v1 );
        b.append( n2, v2 );
        b.append( n3, v3 );
        return b.obj();
    }
};

BOOST_FIXTURE_TEST_CASE(WoCompareBasic, Base)
{
    BOOST_CHECK( basic( "a", 1 ).woCompare( basic( "a", 1 ) ) == 0 );
    BOOST_CHECK( basic( "a", 2 ).woCompare( basic( "a", 1 ) ) > 0 );
    BOOST_CHECK( basic( "a", 1 ).woCompare( basic( "a", 2 ) ) < 0 );
    // field name comparison
    BOOST_CHECK( basic( "a", 1 ).woCompare( basic( "b", 1 ) ) < 0 );
}

BOOST_FIXTURE_TEST_CASE(IsPrefixOf, Base)
{
    {
        BSONObj k = basic("x", 1);
        BOOST_CHECK( ! k.isPrefixOf( basic("a", 1) ) );
        BOOST_CHECK( k.isPrefixOf( basic("x", 1) ) );
        BOOST_CHECK( k.isPrefixOf( basic("x", 1, "a", 1) ) );
        BOOST_CHECK( ! k.isPrefixOf( basic("a", 1, "x", 1) ) );
    }
    {
        BSONObj k = basic("x", 1, "y", 1);
        BOOST_CHECK( ! k.isPrefixOf( basic("x", 1) ) );
        BOOST_CHECK( ! k.isPrefixOf( basic("x", 1, "z", 1) ) );
        BOOST_CHECK( k.isPrefixOf( basic("x", 1, "y", 1) ) );
        BOOST_CHECK( k.isPrefixOf( basic("x", 1, "y", 1, "z", 1 ) ) );
    }
    {
        BSONObj k = basic("x", 1);
        BOOST_CHECK( ! k.isPrefixOf( basic("x", "hi") ) );
        BOOST_CHECK( k.isPrefixOf( basic("x", 1, "a", "hi") ) );
    }
}


BOOST_FIXTURE_TEST_CASE(NumericCompareBasic, Base)
{
    BOOST_CHECK( basic( "a", 1 ).woCompare( basic( "a", 1.0 ) ) == 0 );
}


BOOST_FIXTURE_TEST_CASE(WoCompareEmbeddedObject, Base)
{
    BOOST_CHECK( basic( "a", basic( "b", 1 ) ).woCompare
            ( basic( "a", basic( "b", 1.0 ) ) ) == 0 );
    BOOST_CHECK( basic( "a", basic( "b", 1 ) ).woCompare
            ( basic( "a", basic( "b", 2 ) ) ) < 0 );
}

BOOST_FIXTURE_TEST_CASE(WoCompareEmbeddedArray, Base)
{
    std::vector< int > i;
    i.push_back( 1 );
    i.push_back( 2 );
    std::vector< double > d;
    d.push_back( 1 );
    d.push_back( 2 );
    BOOST_CHECK( basic( "a", i ).woCompare( basic( "a", d ) ) == 0 );

    std::vector< int > j;
    j.push_back( 1 );
    j.push_back( 3 );
    BOOST_CHECK( basic( "a", i ).woCompare( basic( "a", j ) ) < 0 );
}

BOOST_FIXTURE_TEST_CASE(WoCompareOrdered, Base)
{
    BOOST_CHECK( basic( "a", 1 ).woCompare( basic( "a", 1 ), basic( "a", 1 ) ) == 0 );
    BOOST_CHECK( basic( "a", 2 ).woCompare( basic( "a", 1 ), basic( "a", 1 ) ) > 0 );
    BOOST_CHECK( basic( "a", 1 ).woCompare( basic( "a", 2 ), basic( "a", 1 ) ) < 0 );
    BOOST_CHECK( basic( "a", 1 ).woCompare( basic( "a", 1 ), basic( "a", -1 ) ) == 0 );
    BOOST_CHECK( basic( "a", 2 ).woCompare( basic( "a", 1 ), basic( "a", -1 ) ) < 0 );
    BOOST_CHECK( basic( "a", 1 ).woCompare( basic( "a", 2 ), basic( "a", -1 ) ) > 0 );
}

BOOST_FIXTURE_TEST_CASE(WoCompareDifferentLength, Base)
{
    BOOST_CHECK( basic( "a", 1 ).woCompare( basic("a", 1, "b", 1) ) < 0 );
    BOOST_CHECK( basic("a", 1, "b", 1).woCompare( basic("a", 1) ) > 0 );
}

BOOST_FIXTURE_TEST_CASE(WoSortOrder, Base)
{
    BOOST_CHECK( basic( "a", 1 ).woSortOrder( basic("a", 2), basic("b", 1, "a", 1) ) < 0 );
    BOOST_CHECK( fromjson( "{a:null}" ).woSortOrder( basic("b", 1), basic("a", 1) ) == 0 );
}

BOOST_FIXTURE_TEST_CASE(MultiKeySortOrder, Base)
{
    BOOST_CHECK( basic( "x" , "a" ).woCompare( basic("x", "b") ) < 0 );
    BOOST_CHECK( basic( "x" , "b" ).woCompare( basic("x", "a") ) > 0 );

    BOOST_CHECK( basic("x", "a", "y", "a").woCompare( basic("x", "a", "y", "b") ) < 0 );
    BOOST_CHECK( basic("x", "a", "y", "a").woCompare( basic("x", "b", "y", "a") ) < 0 );
    BOOST_CHECK( basic("x", "a", "y", "a").woCompare( basic("x", "b") ) < 0 );

    BOOST_CHECK( basic( "x" , "c" ).woCompare( basic("x", "b", "y", "h") ) > 0 );
    BOOST_CHECK( basic("x", "b", "y", "b").woCompare( basic("x", "c") ) < 0 );

    BSONObj key = basic("x", 1, "y", 1);

    BOOST_CHECK( basic( "x" , "c" ).woSortOrder( basic("x", "b", "y", "h") , key ) > 0 );
    BOOST_CHECK( basic("x", "b", "y", "b").woCompare( basic("x", "c") , key ) < 0 );

    key = basic("", 1, "", 1);

    BOOST_CHECK( basic( "" , "c" ).woSortOrder( basic("", "b", "", "h") , key ) > 0 );
    BOOST_CHECK( basic("", "b", "", "b").woCompare( basic("", "c") , key ) < 0 );

    {
        BSONObjBuilder b;
        b.append( "" , "c" );
        b.appendNull( "" );
        BSONObj o = b.obj();
        BOOST_CHECK( o.woSortOrder( basic("", "b", "", "h") , key ) > 0 );
        BOOST_CHECK( basic("", "b", "", "h").woSortOrder( o , key ) < 0 );

    }

    BOOST_CHECK( basic( "" , "a" ).woCompare( basic("", "a", "", "c") ) < 0 );
    {
        BSONObjBuilder b;
        b.append( "" , "a" );
        b.appendNull( "" );
        BOOST_CHECK(  b.obj().woCompare( basic("", "a", "", "c") ) < 0 );
    }

}

BOOST_FIXTURE_TEST_CASE(Nan, Base)
{
    double inf = std::numeric_limits< double >::infinity();
    double nan = std::numeric_limits< double >::quiet_NaN();
    double nan2 = std::numeric_limits< double >::signaling_NaN();
    BOOST_CHECK( isNaN(nan) );
    BOOST_CHECK( isNaN(nan2) );
    BOOST_CHECK( !isNaN(inf) );

    BOOST_CHECK( basic("a", inf).woCompare( basic("a", inf) ) == 0 );
    BOOST_CHECK( basic("a", inf).woCompare( basic("a", 1) ) > 0 );
    BOOST_CHECK( basic("a", 1).woCompare( basic("a", inf) ) < 0 );

    BOOST_CHECK( basic("a", nan).woCompare( basic("a", nan) ) == 0 );
    BOOST_CHECK( basic("a", nan).woCompare( basic("a", 1) ) < 0 );

    BOOST_CHECK( basic("a", nan).woCompare( basic( "a" , (int64_t)5000000000LL ) ) < 0 );

    BOOST_CHECK( basic("a", 1).woCompare( basic("a", nan) ) > 0 );

    BOOST_CHECK( basic("a", nan2).woCompare( basic("a", nan2) ) == 0 );
    BOOST_CHECK( basic("a", nan2).woCompare( basic("a", 1) ) < 0 );
    BOOST_CHECK( basic("a", 1).woCompare( basic("a", nan2) ) > 0 );

    BOOST_CHECK( basic("a", inf).woCompare( basic("a", nan) ) > 0 );
    BOOST_CHECK( basic("a", inf).woCompare( basic("a", nan2) ) > 0 );
    BOOST_CHECK( basic("a", nan).woCompare( basic("a", nan2) ) == 0 );
}

BOOST_FIXTURE_TEST_CASE(AppendAs, Base)
{
    BSONObjBuilder b;
    {
        BSONObj foo = basic("foo", 1);
        b.appendAs( foo.firstElement(), "bar" );
    }
    BOOST_CHECK_EQUAL( basic("bar", 1), b.done() );
}

BOOST_FIXTURE_TEST_CASE(ArrayAppendAs, Base)
{
    BSONArrayBuilder b;
    {
        BSONObj foo = basic("foo", 1);
        b.appendAs( foo.firstElement(), "3" );
    }
    BSONArray a = b.arr();
    BSONObj expected = basic( "3" , 1 );
    BOOST_CHECK_EQUAL( expected.firstElement(), a[ 3 ] );
    BOOST_CHECK_EQUAL( 4, a.nFields() );
}

BOOST_FIXTURE_TEST_CASE(GetField, Base)
{
    BSONObj o = BSONObjBuilder()
        .append("a",1)
        .append("b", basic("a", 2))
        .append("c", BSONArrayBuilder().append(basic("a", 3)).append(basic("a", 4)).arr())
        .obj();
    BOOST_CHECK_EQUAL( 1 , o.getFieldDotted( "a" ).numberInt() );
    BOOST_CHECK_EQUAL( 2 , o.getFieldDotted( "b.a" ).numberInt() );
    BOOST_CHECK_EQUAL( 3 , o.getFieldDotted( "c.0.a" ).numberInt() );
    BOOST_CHECK_EQUAL( 4 , o.getFieldDotted( "c.1.a" ).numberInt() );
}


static BSONObj recursiveBSON( int depth )
{
    BSONObjBuilder b;
    if ( depth==0 ) {
        b.append("name", "Joe");
        return b.obj();
    }
    b.append( "test", recursiveBSON( depth - 1) );
    return b.obj();
}

BOOST_AUTO_TEST_CASE(ToStringRecursionDepth)
{
    BSONObj nestedBSON;
    StringBuilder s;
    std::string nestedBSONString;
    std::size_t found;

    // recursion depth one less than max allowed-- do not shorten the string
    nestedBSON = recursiveBSON( BSONObj::maxToStringRecursionDepth - 1  );
    nestedBSON.toString( s, true, false );
    nestedBSONString = s.str();
    found = nestedBSONString.find( "..." );
    // did not find the "..." pattern
    BOOST_CHECK_EQUAL( found!=std::string::npos, false );

    // recursion depth is equal to max allowed  -- do not shorten the string
    nestedBSON = recursiveBSON( BSONObj::maxToStringRecursionDepth );
    nestedBSON.toString( s, true, false );
    nestedBSONString = s.str();
    found = nestedBSONString.find( "..." );
    // did not find the "..." pattern
    BOOST_CHECK_EQUAL( found!=std::string::npos, false );

    // recursion depth - one greater than max allowed -- shorten the string
    nestedBSON = recursiveBSON( BSONObj::maxToStringRecursionDepth + 1 );
    nestedBSON.toString( s, false, false );
    nestedBSONString = s.str();
    found = nestedBSONString.find( "..." );
    // found the "..." pattern
    BOOST_CHECK_EQUAL( found!=std::string::npos, true );

    /* recursion depth - one greater than max allowed but with full=true
     * should fail with an assertion
     */
    nestedBSON = recursiveBSON( BSONObj::maxToStringRecursionDepth + 1 );
     // TODO this asserts, needs better control over error reporting
    //BOOST_CHECK_THROW( nestedBSON.toString( s, false, true ) , std::exception );
}

BOOST_AUTO_TEST_SUITE_END()
