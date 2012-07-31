/** @file bsoninlines.h
          a goal here is that the most common bson methods can be used inline-only, a la boost.
          thus some things are inline that wouldn't necessarily be otherwise.
*/

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
#ifndef BSON_OBJBUILDER_IPP
#define BSON_OBJBUILDER_IPP

#include "bson/bsonobjbuilder.hpp"
#include "bson/bsonobjiterator.hpp"
#include "bson/bsonobj.ipp"
#include "bson/optime.hpp"

#include <map>
#include <limits>
#include <cassert>

namespace mongo {

    /* add all the fields from the object specified to this object */
    inline BSONObjBuilder& BSONObjBuilder::appendElements(BSONObj x) {
        BSONObjIterator it(x);
        while ( it.moreWithEOO() ) {
            BSONElement e = it.next();
            if ( e.eoo() ) break;
            append(e);
        }
        return *this;
    }

    /* add all the fields from the object specified to this object if they don't exist */
    inline BSONObjBuilder& BSONObjBuilder::appendElementsUnique(BSONObj x) {
        std::set<std::string> have;
        {
            BSONObjIterator i = iterator();
            while ( i.more() )
                have.insert( i.next().fieldName() );
        }
        
        BSONObjIterator it(x);
        while ( it.more() ) {
            BSONElement e = it.next();
            if ( have.count( e.fieldName() ) )
                continue;
            append(e);
        }
        return *this;
    }

    inline BSONObjIterator BSONObjBuilder::iterator() const {
        const char * s = _b.buf() + _offset;
        const char * e = _b.buf() + _b.len();
        return BSONObjIterator( s , e );
    }

    inline bool BSONObjBuilder::hasField( const StringData& name ) const {
        BSONObjIterator i = iterator();
        while ( i.more() )
            if ( strcmp( name.data() , i.next().fieldName() ) == 0 )
                return true;
        return false;
    }

    inline void BSONObjBuilder::appendKeys( const BSONObj& keyPattern , const BSONObj& values ) {
        BSONObjIterator i(keyPattern);
        BSONObjIterator j(values);

        while ( i.more() && j.more() ) {
            appendAs( j.next() , i.next().fieldName() );
        }

        assert( ! i.more() );
        assert( ! j.more() );
    }

    /**
    Timestamps are a special BSON datatype that is used internally for replication.
    Append a timestamp element to the object being ebuilt.
    @param time - in millis (but stored in seconds)
    */
    inline BSONObjBuilder& BSONObjBuilder::appendTimestamp( const StringData& fieldName , uint64_t time , uint32_t inc ) {
        OpTime t( (unsigned) (time / 1000) , inc );
        appendTimestamp( fieldName , t.asDate() );
        return *this;
    }

} // namespace

#endif // BSON_OBJBUILDER_IPP
