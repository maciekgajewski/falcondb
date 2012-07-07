#define BOOST_TEST_MODULE bson_test
#include <boost/test/included/unit_test.hpp>

#include "bson/bson.hpp"

using namespace mongo;

BOOST_AUTO_TEST_SUITE(bson_test_suite)

BOOST_AUTO_TEST_CASE(regex)
{

    BSONObjBuilder b;
    b.appendRegex("x", "foo");
    BSONObj o = b.done();

    BSONObjBuilder c;
    c.appendRegex("x", "goo");
    BSONObj p = c.done();

    BOOST_CHECK( !o.binaryEqual( p ) );
    BOOST_CHECK_LT( o.woCompare( p ), 0 );

}

BOOST_AUTO_TEST_CASE(oid)
{
    OID id;
    id.init();

    OID b;

    b.init( id.str() );
    BOOST_CHECK_EQUAL( b , id );
}

BOOST_AUTO_TEST_CASE(bounds)
{
    BSONObj l , r;
    {
        BSONObjBuilder b;
        b.append( "x" , std::numeric_limits<long long>::max() );
        l = b.obj();
    }
    {
        BSONObjBuilder b;
        b.append( "x" , std::numeric_limits<double>::max() );
        r = b.obj();
    }
    BOOST_CHECK_LT( l.woCompare( r ) , 0 );
    BOOST_CHECK_GT( r.woCompare( l ) , 0 );
    {
        BSONObjBuilder b;
        b.append( "x" , std::numeric_limits<int>::max() );
        l = b.obj();
    }
    BOOST_CHECK_LT( l.woCompare( r ) , 0 );
    BOOST_CHECK_GT( r.woCompare( l ) , 0 );
}

BOOST_AUTO_TEST_CASE(order)
{
    {
        BSONObj x,y,z;
        { BSONObjBuilder b; b.append( "x" , (long long)2 ); x = b.obj(); }
        { BSONObjBuilder b; b.append( "x" , (int)3 ); y = b.obj(); }
        { BSONObjBuilder b; b.append( "x" , (long long)4 ); z = b.obj(); }
        BOOST_CHECK_LT( x.woCompare( y ) , 0 );
        BOOST_CHECK_LT( x.woCompare( z ) , 0 );
        BOOST_CHECK_GT( y.woCompare( x ) , 0 );
        BOOST_CHECK_GT( z.woCompare( x ) , 0 );
        BOOST_CHECK_LT( y.woCompare( z ) , 0 );
        BOOST_CHECK_GT( z.woCompare( y ) , 0 );
    }

    {
        BSONObj ll,d,i,n,u;
        { BSONObjBuilder b; b.append( "x" , (long long)2 ); ll = b.obj(); }
        { BSONObjBuilder b; b.append( "x" , (double)2 ); d = b.obj(); }
        { BSONObjBuilder b; b.append( "x" , (int)2 ); i = b.obj(); }
        { BSONObjBuilder b; b.appendNull( "x" ); n = b.obj(); }
        { BSONObjBuilder b; u = b.obj(); }

        BOOST_CHECK_EQUAL( ll.woCompare( u ) , d.woCompare( u ) );
        BOOST_CHECK_EQUAL( ll.woCompare( u ) , i.woCompare( u ) );
        BSONObj k = BSON( "x" << 1 );
        BOOST_CHECK_EQUAL( ll.woCompare( u , k ) , d.woCompare( u , k ) );
        BOOST_CHECK_EQUAL( ll.woCompare( u , k ) , i.woCompare( u , k ) );

        BOOST_CHECK_EQUAL( u.woCompare( ll ) , u.woCompare( d ) );
        BOOST_CHECK_EQUAL( u.woCompare( ll ) , u.woCompare( i ) );
        BOOST_CHECK_EQUAL( u.woCompare( ll , k ) , u.woCompare( d , k ) );
        BOOST_CHECK_EQUAL( u.woCompare( ll , k ) , u.woCompare( d , k ) );

        BOOST_CHECK_EQUAL( i.woCompare( n ) , d.woCompare( n ) );

        BOOST_CHECK_EQUAL( ll.woCompare( n ) , d.woCompare( n ) );
        BOOST_CHECK_EQUAL( ll.woCompare( n ) , i.woCompare( n ) );
        BOOST_CHECK_EQUAL( ll.woCompare( n , k ) , d.woCompare( n , k ) );
        BOOST_CHECK_EQUAL( ll.woCompare( n , k ) , i.woCompare( n , k ) );

        BOOST_CHECK_EQUAL( n.woCompare( ll ) , n.woCompare( d ) );
        BOOST_CHECK_EQUAL( n.woCompare( ll ) , n.woCompare( i ) );
        BOOST_CHECK_EQUAL( n.woCompare( ll , k ) , n.woCompare( d , k ) );
        BOOST_CHECK_EQUAL( n.woCompare( ll , k ) , n.woCompare( d , k ) );
    }

    {
        BSONObj l,r;
        { BSONObjBuilder b; b.append( "x" , "eliot" ); l = b.obj(); }
        { BSONObjBuilder b; b.appendSymbol( "x" , "eliot" ); r = b.obj(); }
        BOOST_CHECK_EQUAL( l.woCompare( r ) , 0 );
        BOOST_CHECK_EQUAL( r.woCompare( l ) , 0 );
    }
}

BOOST_AUTO_TEST_CASE(other)
{
    BSONObjBuilder A,B,C;
    A.append("x", 2);
    B.append("x", 2.0);
    C.append("x", 2.1);
    BSONObj a = A.done();
    BSONObj b = B.done();
    BSONObj c = C.done();
    BOOST_CHECK( !a.binaryEqual( b ) );
    int cmp = a.woCompare(b);
    BOOST_CHECK_EQUAL( cmp , 0 );
    cmp = a.woCompare(c);
    BOOST_CHECK_LT( cmp , 0 );
}

BOOST_AUTO_TEST_SUITE_END()
