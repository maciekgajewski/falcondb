// @file bsonmisc.h

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

#include "bson/bsonelement.hpp"

#include <memory>

namespace mongo {

    int getGtLtOp(const BSONElement& e);

    struct BSONElementCmpWithoutField {
        bool operator()( const BSONElement &l, const BSONElement &r ) const {
            return l.woCompare( r, false ) < 0;
        }
    };

    class BSONObjCmp {
    public:
        BSONObjCmp( const BSONObj &order = BSONObj() ) : _order( order ) {}
        bool operator()( const BSONObj &l, const BSONObj &r ) const {
            return l.woCompare( r, _order ) < 0;
        }
        BSONObj order() const { return _order; }
    private:
        BSONObj _order;
    };

    typedef std::set<BSONObj,BSONObjCmp> BSONObjSet;

    enum FieldCompareResult {
        LEFT_SUBFIELD = -2,
        LEFT_BEFORE = -1,
        SAME = 0,
        RIGHT_BEFORE = 1 ,
        RIGHT_SUBFIELD = 2
    };
    /**
       used in conjuction with BSONObjBuilder, allows for proper buffer size to prevent crazy memory usage
     */
    class BSONSizeTracker {
    public:
        BSONSizeTracker() {
            _pos = 0;
            for ( int i=0; i<SIZE; i++ )
                _sizes[i] = 512; // this is the default, so just be consistent
        }

        ~BSONSizeTracker() {
        }

        void got( int size ) {
            _sizes[_pos++] = size;
            if ( _pos >= SIZE )
                _pos = 0;
        }

        /**
         * right now choosing largest size
         */
        int getSize() const {
            int x = 16; // sane min
            for ( int i=0; i<SIZE; i++ ) {
                if ( _sizes[i] > x )
                    x = _sizes[i];
            }
            return x;
        }

    private:
        enum { SIZE = 10 };
        int _pos;
        int _sizes[SIZE];
    };
}
