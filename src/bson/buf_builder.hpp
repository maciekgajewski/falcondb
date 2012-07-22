/*    Copyright 2009 10gen Inc.
 *    Copyright (C) 2012 Maciej Gajewski <maciej.gajewski0 at gmail dot com>
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
#ifndef BSON_BUF_BUILDER_HPP
#define BSON_BUF_BUILDER_HPP

#include "bson/allocators.hpp"

#include <cassert>

namespace mongo
{

template<typename Allocator>
class StringBuilderImpl;

template< class Allocator >
class _BufBuilder {
    // non-copyable, non-assignable
    _BufBuilder( const _BufBuilder& );
    _BufBuilder& operator=( const _BufBuilder& );
    Allocator al;
public:
    _BufBuilder(int initsize = 512) : size(initsize) {
        if ( size > 0 ) {
            data = (char *) al.Malloc(size);
            assert(data); // "out of memory BufBuilder");
        }
        else {
            data = 0;
        }
        l = 0;
    }
    ~_BufBuilder() { kill(); }

    void kill() {
        if ( data ) {
            al.Free(data);
            data = 0;
        }
    }

    void reset() {
        l = 0;
    }
    void reset( int maxSize ) {
        l = 0;
        if ( maxSize && size > maxSize ) {
            al.Free(data);
            data = (char*)al.Malloc(maxSize);
            assert(data);
            size = maxSize;
        }
    }

    /** leave room for some stuff later
        @return point to region that was skipped.  pointer may change later (on realloc), so for immediate use only
    */
    char* skip(int n) { return grow(n); }

    /* note this may be deallocated (realloced) if you keep writing. */
    char* buf() { return data; }
    const char* buf() const { return data; }

    /* assume ownership of the buffer - you must then free() it */
    void decouple() { data = 0; }

    void appendUChar(unsigned char j) {
        *((unsigned char*)grow(sizeof(unsigned char))) = j;
    }
    void appendChar(char j) {
        *((char*)grow(sizeof(char))) = j;
    }
    void appendNum(char j) {
        *((char*)grow(sizeof(char))) = j;
    }
    void appendNum(int16_t j) {
        *((short*)grow(sizeof(short))) = j;
    }
    void appendNum(int32_t j) {
        *((int32_t*)grow(sizeof(int32_t))) = j;
    }
    void appendNum(uint32_t j) {
        *((uint32_t*)grow(sizeof(uint32_t))) = j;
    }
    void appendNum(bool j) {
        *((bool*)grow(sizeof(bool))) = j;
    }
    void appendNum(double j) {
        *((double*)(grow(sizeof(double)))) = j;
    }
    void appendNum(int64_t j) {
        *((int64_t*)grow(sizeof(int64_t))) = j;
    }
    void appendNum(uint64_t j) {
        *((uint64_t*)grow(sizeof(uint64_t))) = j;
    }

    void appendBuf(const void *src, size_t len) {
        memcpy(grow((int) len), src, len);
    }

    template<class T>
    void appendStruct(const T& s) {
        appendBuf(&s, sizeof(T));
    }

    void appendStr(const StringData &str , bool includeEndingNull = true ) {
        const int len = str.size() + ( includeEndingNull ? 1 : 0 );
        memcpy(grow(len), str.data(), len);
    }

    /** @return length of current string */
    int len() const { return l; }
    void setlen( int newLen ) { l = newLen; }
    /** @return size of the buffer */
    int getSize() const { return size; }

    /* returns the pre-grow write position */
    inline char* grow(int by) {
        int oldlen = l;
        l += by;
        if ( l > size ) {
            grow_reallocate();
        }
        return data + oldlen;
    }

private:
    /* "slow" portion of 'grow()'  */
    void grow_reallocate() {
        int a = 64;
        while( a < l )
            a = a * 2;
        data = (char *) al.Realloc(data, a);
        assert(data);
        if ( data == NULL )
        size = a;
    }

    char *data;
    int l;
    int size;

    friend class StringBuilderImpl<Allocator>;
};

typedef _BufBuilder<TrivialAllocator> BufBuilder;

/** The StackBufBuilder builds smaller datasets on the stack instead of using malloc.
      this can be significantly faster for small bufs.  However, you can not decouple() the
      buffer with StackBufBuilder.
    While designed to be a variable on the stack, if you were to dynamically allocate one,
      nothing bad would happen.  In fact in some circumstances this might make sense, say,
      embedded in some other object.
*/
class StackBufBuilder : public _BufBuilder<StackAllocator> {
public:
    StackBufBuilder() : _BufBuilder<StackAllocator>(StackAllocator::SZ) { }
    void decouple(); // not allowed. not implemented.
};

} // namespace


#endif // BSON_BUF_BUILDER_HPP
