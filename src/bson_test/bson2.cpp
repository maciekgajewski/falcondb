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
    static BSONObj basic( const char *name, int val ) {
        BSONObjBuilder b;
        b.append( name, val );
        return b.obj();
    }
    static BSONObj basic( const char *name, std::vector< int > val ) {
        BSONObjBuilder b;
        b.append( name, val );
        return b.obj();
    }
    template< class T >
    static BSONObj basic( const char *name, T val ) {
        BSONObjBuilder b;
        b.append( name, val );
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
        BSONObj k = BSON( "x" << 1 );
        BOOST_CHECK( ! k.isPrefixOf( BSON( "a" << 1 ) ) );
        BOOST_CHECK( k.isPrefixOf( BSON( "x" << 1 ) ) );
        BOOST_CHECK( k.isPrefixOf( BSON( "x" << 1 << "a" << 1 ) ) );
        BOOST_CHECK( ! k.isPrefixOf( BSON( "a" << 1 << "x" << 1 ) ) );
    }
    {
        BSONObj k = BSON( "x" << 1 << "y" << 1 );
        BOOST_CHECK( ! k.isPrefixOf( BSON( "x" << 1 ) ) );
        BOOST_CHECK( ! k.isPrefixOf( BSON( "x" << 1 << "z" << 1 ) ) );
        BOOST_CHECK( k.isPrefixOf( BSON( "x" << 1 << "y" << 1 ) ) );
        BOOST_CHECK( k.isPrefixOf( BSON( "x" << 1 << "y" << 1 << "z" << 1 ) ) );
    }
    {
        BSONObj k = BSON( "x" << 1 );
        BOOST_CHECK( ! k.isPrefixOf( BSON( "x" << "hi" ) ) );
        BOOST_CHECK( k.isPrefixOf( BSON( "x" << 1 << "a" << "hi" ) ) );
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
    BOOST_CHECK( BSON( "a" << 1 ).woCompare( BSON( "a" << 1 << "b" << 1 ) ) < 0 );
    BOOST_CHECK( BSON( "a" << 1 << "b" << 1 ).woCompare( BSON( "a" << 1 ) ) > 0 );
}

BOOST_FIXTURE_TEST_CASE(WoSortOrder, Base)
{
    BOOST_CHECK( BSON( "a" << 1 ).woSortOrder( BSON( "a" << 2 ), BSON( "b" << 1 << "a" << 1 ) ) < 0 );
    BOOST_CHECK( fromjson( "{a:null}" ).woSortOrder( BSON( "b" << 1 ), BSON( "a" << 1 ) ) == 0 );
}

BOOST_FIXTURE_TEST_CASE(MultiKeySortOrder, Base)
{
    BOOST_CHECK( BSON( "x" << "a" ).woCompare( BSON( "x" << "b" ) ) < 0 );
    BOOST_CHECK( BSON( "x" << "b" ).woCompare( BSON( "x" << "a" ) ) > 0 );

    BOOST_CHECK( BSON( "x" << "a" << "y" << "a" ).woCompare( BSON( "x" << "a" << "y" << "b" ) ) < 0 );
    BOOST_CHECK( BSON( "x" << "a" << "y" << "a" ).woCompare( BSON( "x" << "b" << "y" << "a" ) ) < 0 );
    BOOST_CHECK( BSON( "x" << "a" << "y" << "a" ).woCompare( BSON( "x" << "b" ) ) < 0 );

    BOOST_CHECK( BSON( "x" << "c" ).woCompare( BSON( "x" << "b" << "y" << "h" ) ) > 0 );
    BOOST_CHECK( BSON( "x" << "b" << "y" << "b" ).woCompare( BSON( "x" << "c" ) ) < 0 );

    BSONObj key = BSON( "x" << 1 << "y" << 1 );

    BOOST_CHECK( BSON( "x" << "c" ).woSortOrder( BSON( "x" << "b" << "y" << "h" ) , key ) > 0 );
    BOOST_CHECK( BSON( "x" << "b" << "y" << "b" ).woCompare( BSON( "x" << "c" ) , key ) < 0 );

    key = BSON( "" << 1 << "" << 1 );

    BOOST_CHECK( BSON( "" << "c" ).woSortOrder( BSON( "" << "b" << "" << "h" ) , key ) > 0 );
    BOOST_CHECK( BSON( "" << "b" << "" << "b" ).woCompare( BSON( "" << "c" ) , key ) < 0 );

    {
        BSONObjBuilder b;
        b.append( "" , "c" );
        b.appendNull( "" );
        BSONObj o = b.obj();
        BOOST_CHECK( o.woSortOrder( BSON( "" << "b" << "" << "h" ) , key ) > 0 );
        BOOST_CHECK( BSON( "" << "b" << "" << "h" ).woSortOrder( o , key ) < 0 );

    }

    BOOST_CHECK( BSON( "" << "a" ).woCompare( BSON( "" << "a" << "" << "c" ) ) < 0 );
    {
        BSONObjBuilder b;
        b.append( "" , "a" );
        b.appendNull( "" );
        BOOST_CHECK(  b.obj().woCompare( BSON( "" << "a" << "" << "c" ) ) < 0 ); // SERVER-282
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

    BOOST_CHECK( BSON( "a" << inf ).woCompare( BSON( "a" << inf ) ) == 0 );
    BOOST_CHECK( BSON( "a" << inf ).woCompare( BSON( "a" << 1 ) ) > 0 );
    BOOST_CHECK( BSON( "a" << 1 ).woCompare( BSON( "a" << inf ) ) < 0 );

    BOOST_CHECK( BSON( "a" << nan ).woCompare( BSON( "a" << nan ) ) == 0 );
    BOOST_CHECK( BSON( "a" << nan ).woCompare( BSON( "a" << 1 ) ) < 0 );

    BOOST_CHECK( BSON( "a" << nan ).woCompare( BSON( "a" << 5000000000LL ) ) < 0 );

    BOOST_CHECK( BSON( "a" << 1 ).woCompare( BSON( "a" << nan ) ) > 0 );

    BOOST_CHECK( BSON( "a" << nan2 ).woCompare( BSON( "a" << nan2 ) ) == 0 );
    BOOST_CHECK( BSON( "a" << nan2 ).woCompare( BSON( "a" << 1 ) ) < 0 );
    BOOST_CHECK( BSON( "a" << 1 ).woCompare( BSON( "a" << nan2 ) ) > 0 );

    BOOST_CHECK( BSON( "a" << inf ).woCompare( BSON( "a" << nan ) ) > 0 );
    BOOST_CHECK( BSON( "a" << inf ).woCompare( BSON( "a" << nan2 ) ) > 0 );
    BOOST_CHECK( BSON( "a" << nan ).woCompare( BSON( "a" << nan2 ) ) == 0 );
}

BOOST_AUTO_TEST_CASE(AppendAs)
{
    BSONObjBuilder b;
    {
        BSONObj foo = BSON( "foo" << 1 );
        b.appendAs( foo.firstElement(), "bar" );
    }
    BOOST_CHECK_EQUAL( BSON( "bar" << 1 ), b.done() );
}

BOOST_AUTO_TEST_CASE(ArrayAppendAs)
{
    BSONArrayBuilder b;
    {
        BSONObj foo = BSON( "foo" << 1 );
        b.appendAs( foo.firstElement(), "3" );
    }
    BSONArray a = b.arr();
    BSONObj expected = BSON( "3" << 1 );
    BOOST_CHECK_EQUAL( expected.firstElement(), a[ 3 ] );
    BOOST_CHECK_EQUAL( 4, a.nFields() );
}

BOOST_AUTO_TEST_CASE(GetField)
{
    BSONObj o = BSON( "a" << 1 <<
                      "b" << BSON( "a" << 2 ) <<
                      "c" << BSON_ARRAY( BSON( "a" << 3 ) << BSON( "a" << 4 ) ) );
    BOOST_CHECK_EQUAL( 1 , o.getFieldDotted( "a" ).numberInt() );
    BOOST_CHECK_EQUAL( 2 , o.getFieldDotted( "b.a" ).numberInt() );
    BOOST_CHECK_EQUAL( 3 , o.getFieldDotted( "c.0.a" ).numberInt() );
    BOOST_CHECK_EQUAL( 4 , o.getFieldDotted( "c.1.a" ).numberInt() );
}


static BSONObj recursiveBSON( int depth )
{
    BSONObjBuilder b;
    if ( depth==0 ) {
        b << "name" << "Joe";
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
    BOOST_CHECK_EQUAL( found!=string::npos, false );

    // recursion depth is equal to max allowed  -- do not shorten the string
    nestedBSON = recursiveBSON( BSONObj::maxToStringRecursionDepth );
    nestedBSON.toString( s, true, false );
    nestedBSONString = s.str();
    found = nestedBSONString.find( "..." );
    // did not find the "..." pattern
    BOOST_CHECK_EQUAL( found!=string::npos, false );

    // recursion depth - one greater than max allowed -- shorten the string
    nestedBSON = recursiveBSON( BSONObj::maxToStringRecursionDepth + 1 );
    nestedBSON.toString( s, false, false );
    nestedBSONString = s.str();
    found = nestedBSONString.find( "..." );
    // found the "..." pattern
    BOOST_CHECK_EQUAL( found!=string::npos, true );

    /* recursion depth - one greater than max allowed but with full=true
     * should fail with an assertion
     */
    nestedBSON = recursiveBSON( BSONObj::maxToStringRecursionDepth + 1 );
     // TODO this asserts, needs better control over error reporting
    //BOOST_CHECK_THROW( nestedBSON.toString( s, false, true ) , std::exception );
}

BOOST_AUTO_TEST_SUITE_END()
