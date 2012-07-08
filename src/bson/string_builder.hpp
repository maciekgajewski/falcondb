/* builder.h */

/*    Copyright 2009 10gen Inc.
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

#pragma once
#ifndef BSON_STRING_BUILDER_HPP
#define BSON_STRING_BUILDER_HPP

#include <cfloat>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <string.h>
#include <cassert>

#include "bson/string_data.hpp"

#include "bson/buf_builder.hpp"
#include "bson/allocators.hpp"

namespace mongo {

/** stringstream deals with locale so this is a lot faster than std::stringstream for UTF8 */
template <typename Allocator>
class StringBuilderImpl {
public:
    static const size_t MONGO_DBL_SIZE = 3 + DBL_MANT_DIG - DBL_MIN_EXP;
    static const size_t MONGO_S32_SIZE = 12;
    static const size_t MONGO_U32_SIZE = 11;
    static const size_t MONGO_S64_SIZE = 23;
    static const size_t MONGO_U64_SIZE = 22;
    static const size_t MONGO_S16_SIZE = 7;

    StringBuilderImpl() { }

    StringBuilderImpl& operator<<( double x ) {
        return SBNUM( x , MONGO_DBL_SIZE , "%g" );
    }
    StringBuilderImpl& operator<<( int x ) {
        return SBNUM( x , MONGO_S32_SIZE , "%d" );
    }
    StringBuilderImpl& operator<<( unsigned x ) {
        return SBNUM( x , MONGO_U32_SIZE , "%u" );
    }
    StringBuilderImpl& operator<<( long x ) {
        return SBNUM( x , MONGO_S64_SIZE , "%ld" );
    }
    StringBuilderImpl& operator<<( unsigned long x ) {
        return SBNUM( x , MONGO_U64_SIZE , "%lu" );
    }
    StringBuilderImpl& operator<<( long long x ) {
        return SBNUM( x , MONGO_S64_SIZE , "%lld" );
    }
    StringBuilderImpl& operator<<( unsigned long long x ) {
        return SBNUM( x , MONGO_U64_SIZE , "%llu" );
    }
    StringBuilderImpl& operator<<( short x ) {
        return SBNUM( x , MONGO_S16_SIZE , "%hd" );
    }
    StringBuilderImpl& operator<<( char c ) {
        _buf.grow( 1 )[0] = c;
        return *this;
    }

    void appendDoubleNice( double x ) {
        const int prev = _buf.l;
        const int maxSize = 32;
        char * start = _buf.grow( maxSize );
        int z = snprintf( start , maxSize , "%.16g" , x );
        assert( z >= 0 );
        assert( z < maxSize );
        _buf.l = prev + z;
        if( strchr(start, '.') == 0 && strchr(start, 'E') == 0 && strchr(start, 'N') == 0 ) {
            write( ".0" , 2 );
        }
    }

    void write( const char* buf, int len) { memcpy( _buf.grow( len ) , buf , len ); }

    void append( const StringData& str ) { memcpy( _buf.grow( str.size() ) , str.data() , str.size() ); }

    StringBuilderImpl& operator<<( const StringData& str ) {
        append( str );
        return *this;
    }

    void reset( int maxSize = 0 ) { _buf.reset( maxSize ); }

    std::string str() const { return std::string(_buf.data, _buf.l); }

    int len() const { return _buf.l; }

private:
    _BufBuilder<Allocator> _buf;

    // non-copyable, non-assignable
    StringBuilderImpl( const StringBuilderImpl& );
    StringBuilderImpl& operator=( const StringBuilderImpl& );

    template <typename T>
    StringBuilderImpl& SBNUM(T val,int maxSize,const char *macro)  {
        int prev = _buf.l;
        int z = snprintf( _buf.grow(maxSize) , maxSize , macro , (val) );
        assert( z >= 0 );
        assert( z < maxSize );
        _buf.l = prev + z;
        return *this;
    }
};

typedef StringBuilderImpl<TrivialAllocator> StringBuilder;
typedef StringBuilderImpl<StackAllocator> StackStringBuilder;

} // namespace mongo


#endif // BSON_STRING_BUILDER_HPP

