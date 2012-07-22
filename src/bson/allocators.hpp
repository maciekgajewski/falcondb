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


#ifndef BSON_ALLOCATORS_HPP
#define BSON_ALLOCATORS_HPP

#include <cassert>
#include <cstring>

namespace mongo
{

// TODO replace with std::allocator
class TrivialAllocator {
public:
    void* Malloc(size_t sz) { return ::malloc(sz); }
    void* Realloc(void *p, size_t sz) { return ::realloc(p, sz); }
    void Free(void *p) { ::free(p); }
};

class StackAllocator {
public:
    enum { SZ = 512 };

    void* Malloc(size_t sz) {
        if( sz <= SZ ) return buf;
        return ::malloc(sz);
    }

    void* Realloc(void *p, size_t sz) {
        if( p == buf ) {
            if( sz <= SZ ) return buf;
            void *d = malloc(sz);
            assert(d);// "out of memory StackAllocator::Realloc" );
            std::memcpy(d, p, SZ);
            return d;
        }
        return ::realloc(p, sz);
    }

    void Free(void *p) {
        if( p != buf )
            ::free(p);
    }
private:
    char buf[SZ];
};


}

#endif // BSON_ALLOCATORS_HPP
