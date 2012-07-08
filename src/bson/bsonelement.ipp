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
#ifndef BSON_ELEMENT_IPP
#define BSON_ELEMENT_IPP

#include "bson/bsonelement.hpp"
#include "bson/bsonobj.hpp"
#include "bson/bsonobjbuilder.hpp"
#include "bson/bsonobjiterator.hpp"
#include "bson/optime.hpp"

#include <map>
#include <limits>
#include <cassert>


namespace mongo {

    /* must be same type when called, unless both sides are #s 
       this large function is in header to facilitate inline-only use of bson
    */
    inline int compareElementValues(const BSONElement& l, const BSONElement& r) {
        int f;

        switch ( l.type() ) {
        case EOO:
        case Undefined: // EOO and Undefined are same canonicalType
        case jstNULL:
        case MaxKey:
        case MinKey:
            f = l.canonicalType() - r.canonicalType();
            if ( f<0 ) return -1;
            return f==0 ? 0 : 1;
        case Bool:
            return *l.value() - *r.value();
        case Timestamp:
            // unsigned compare for timestamps - note they are not really dates but (ordinal + time_t)
            if ( l.date() < r.date() )
                return -1;
            return l.date() == r.date() ? 0 : 1;
        case Date:
            {
                long long a = (long long) l.Date().millis;
                long long b = (long long) r.Date().millis;
                if( a < b ) 
                    return -1;
                return a == b ? 0 : 1;
            }
        case NumberLong:
            if( r.type() == NumberLong ) {
                long long L = l._numberLong();
                long long R = r._numberLong();
                if( L < R ) return -1;
                if( L == R ) return 0;
                return 1;
            }
            goto dodouble;
        case NumberInt:
            if( r.type() == NumberInt ) {
                int L = l._numberInt();
                int R = r._numberInt();
                if( L < R ) return -1;
                return L == R ? 0 : 1;
            }
            // else fall through
        case NumberDouble: 
dodouble:
            {
                double left = l.number();
                double right = r.number();
                if( left < right ) 
                    return -1;
                if( left == right )
                    return 0;
                if( isNaN(left) )
                    return isNaN(right) ? 0 : -1;
                return 1;
            }
        case jstOID:
            return memcmp(l.value(), r.value(), 12);
        case Code:
        case Symbol:
        case String:
            /* todo: a utf sort order version one day... */
            {
                // we use memcmp as we allow zeros in UTF8 strings
                int lsz = l.valuestrsize();
                int rsz = r.valuestrsize();
                int common = std::min(lsz, rsz);
                int res = memcmp(l.valuestr(), r.valuestr(), common);
                if( res ) 
                    return res;
                // longer string is the greater one
                return lsz-rsz;
            }
        case Object:
        case Array:
            return l.embeddedObject().woCompare( r.embeddedObject() );
        case DBRef: {
            int lsz = l.valuesize();
            int rsz = r.valuesize();
            if ( lsz - rsz != 0 ) return lsz - rsz;
            return memcmp(l.value(), r.value(), lsz);
        }
        case BinData: {
            int lsz = l.objsize(); // our bin data size in bytes, not including the subtype byte
            int rsz = r.objsize();
            if ( lsz - rsz != 0 ) return lsz - rsz;
            return memcmp(l.value()+4, r.value()+4, lsz+1 /*+1 for subtype byte*/);
        }
        case RegEx: {
            int c = strcmp(l.regex(), r.regex());
            if ( c )
                return c;
            return strcmp(l.regexFlags(), r.regexFlags());
        }
        case CodeWScope : {
            f = l.canonicalType() - r.canonicalType();
            if ( f )
                return f;
            f = strcmp( l.codeWScopeCode() , r.codeWScopeCode() );
            if ( f )
                return f;
            f = strcmp( l.codeWScopeScopeDataUnsafe() , r.codeWScopeScopeDataUnsafe() );
            if ( f )
                return f;
            return 0;
        }
        default:
            assert( false);
        }
        return -1;
    }

    /* wo = "well ordered" 
       note: (mongodb related) : this can only change in behavior when index version # changes
    */
    inline int BSONElement::woCompare( const BSONElement &e,
                                bool considerFieldName ) const {
        int lt = (int) canonicalType();
        int rt = (int) e.canonicalType();
        int x = lt - rt;
        if( x != 0 && (!isNumber() || !e.isNumber()) )
            return x;
        if ( considerFieldName ) {
            x = strcmp(fieldName(), e.fieldName());
            if ( x != 0 )
                return x;
        }
        x = compareElementValues(*this, e);
        return x;
    }

    inline BSONObj BSONElement::embeddedObjectUserCheck() const {
        if ( isABSONObj() )
            return BSONObj(value());
        std::stringstream ss;
        ss << "invalid parameter: expected an object (" << fieldName() << ")";
        std::cerr <<  ss.str() << std::endl;
        assert(false);
        return BSONObj(); // never reachable
    }

    inline BSONObj BSONElement::embeddedObject() const {
        assert( isABSONObj() );
        return BSONObj(value());
    }

    inline BSONObj BSONElement::codeWScopeObject() const {
        assert( type() == CodeWScope );
        int strSizeWNull = *(int *)( value() + 4 );
        return BSONObj( value() + 4 + 4 + strSizeWNull );
    }

    // wrap this element up as a singleton object.
    inline BSONObj BSONElement::wrap() const {
        BSONObjBuilder b(size()+6);
        b.append(*this);
        return b.obj();
    }

    inline BSONObj BSONElement::wrap( const char * newName ) const {
        BSONObjBuilder b(size()+6+(int)strlen(newName));
        b.appendAs(*this,newName);
        return b.obj();
    }

    struct BSONElementFieldNameCmp {
        bool operator()( const BSONElement &l, const BSONElement &r ) const {
            return strcmp( l.fieldName() , r.fieldName() ) <= 0;
        }
    };

    typedef std::set<BSONElement, BSONElementFieldNameCmp> BSONSortedElements;
    inline BSONSortedElements bson2set( const BSONObj& obj ) {
        BSONSortedElements s;
        BSONObjIterator it(obj);
        while ( it.more() )
            s.insert( it.next() );
        return s;
    }

    inline void BSONElement::validate() const {
        const BSONType t = type();

        switch( t ) {
        case DBRef:
        case Code:
        case Symbol:
        case mongo::String: {
            unsigned x = (unsigned) valuestrsize();
            if( valuestr()[x-1] == 0 )
                return;
            assert(false);
            break;
        }
        case CodeWScope: {
            int totalSize = *( int * )( value() );
            assert(totalSize >= 8 );
            int strSizeWNull = *( int * )( value() + 4 );
            assert( totalSize >= strSizeWNull + 4 + 4 );
            assert(  strSizeWNull > 0 &&
                     (strSizeWNull - 1) == mongo::strnlen( codeWScopeCode(), strSizeWNull ) );
            assert( totalSize >= strSizeWNull + 4 + 4 + 4 );
            int objSize = *( int * )( value() + 4 + 4 + strSizeWNull );
            assert( totalSize == 4 + 4 + strSizeWNull + objSize );
            // Subobject validation handled elsewhere.
        }
        case Object:
            // We expect Object size validation to be handled elsewhere.
        default:
            break;
        }
    }

    inline int BSONElement::size( int maxLen ) const {
        if ( totalSize >= 0 )
            return totalSize;

        int remain = maxLen - fieldNameSize() - 1;

        int x = 0;
        switch ( type() ) {
        case EOO:
        case Undefined:
        case jstNULL:
        case MaxKey:
        case MinKey:
            break;
        case mongo::Bool:
            x = 1;
            break;
        case NumberInt:
            x = 4;
            break;
        case Timestamp:
        case mongo::Date:
        case NumberDouble:
        case NumberLong:
            x = 8;
            break;
        case jstOID:
            x = 12;
            break;
        case Symbol:
        case Code:
        case mongo::String:
            assert(maxLen == -1 || remain > 3 );
            x = valuestrsize() + 4;
            break;
        case CodeWScope:
            assert( maxLen == -1 || remain > 3 );
            x = objsize();
            break;

        case DBRef:
            assert( maxLen == -1 || remain > 3 );
            x = valuestrsize() + 4 + 12;
            break;
        case Object:
        case mongo::Array:
            assert( maxLen == -1 || remain > 3 );
            x = objsize();
            break;
        case BinData:
            assert(maxLen == -1 || remain > 3 );
            x = valuestrsize() + 4 + 1/*subtype*/;
            break;
        case RegEx: {
            const char *p = value();
            size_t len1 = ( maxLen == -1 ) ? std::strlen( p ) : std::strlen( p );
            //massert( 10318 ,  "Invalid regex string", len1 != -1 ); // ERH - 4/28/10 - don't think this does anything
            p = p + len1 + 1;
            size_t len2;
            if( maxLen == -1 )
                len2 = strlen( p );
            else {
                size_t x = remain - len1 - 1;
                assert( x <= 0x7fffffff );
                len2 = std::strlen( p );
            }
            //massert( 10319 ,  "Invalid regex options string", len2 != -1 ); // ERH - 4/28/10 - don't think this does anything
            x = (int) (len1 + 1 + len2 + 1);
        }
        break;
        default: {
            StringBuilder ss;
            ss << "BSONElement: bad type " << (int) type();
            std::cerr << ss.str() << std::endl;
            assert( false);
        }
        }
        totalSize =  x + fieldNameSize() + 1; // BSONType

        return totalSize;
    }

    inline int BSONElement::size() const {
        if ( totalSize >= 0 )
            return totalSize;

        int x = 0;
        switch ( type() ) {
        case EOO:
        case Undefined:
        case jstNULL:
        case MaxKey:
        case MinKey:
            break;
        case mongo::Bool:
            x = 1;
            break;
        case NumberInt:
            x = 4;
            break;
        case Timestamp:
        case mongo::Date:
        case NumberDouble:
        case NumberLong:
            x = 8;
            break;
        case jstOID:
            x = 12;
            break;
        case Symbol:
        case Code:
        case mongo::String:
            x = valuestrsize() + 4;
            break;
        case DBRef:
            x = valuestrsize() + 4 + 12;
            break;
        case CodeWScope:
        case Object:
        case mongo::Array:
            x = objsize();
            break;
        case BinData:
            x = valuestrsize() + 4 + 1/*subtype*/;
            break;
        case RegEx: 
            {
                const char *p = value();
                size_t len1 = strlen(p);
                p = p + len1 + 1;
                size_t len2;
                len2 = strlen( p );
                x = (int) (len1 + 1 + len2 + 1);
            }
            break;
        default: 
            {
                StringBuilder ss;
                ss << "BSONElement: bad type " << (int) type();
                std::cerr <<  ss.str() << std::endl;
                assert(false);
            }
        }
        totalSize =  x + fieldNameSize() + 1; // BSONType

        return totalSize;
    }

    inline std::string BSONElement::toString( bool includeFieldName, bool full ) const {
        StringBuilder s;
        toString(s, includeFieldName, full);
        return s.str();
    }
    inline void BSONElement::toString( StringBuilder& s, bool includeFieldName, bool full, int depth ) const {

        if ( depth > BSONObj::maxToStringRecursionDepth ) {
            // check if we want the full/complete string
            if ( full ) {
                StringBuilder s;
                s << "Reached maximum recursion depth of ";
                s << BSONObj::maxToStringRecursionDepth;
                std::cerr << s.str() << std::endl;
                assert(full != true);
            }
            s << "...";
            return;
        }

        if ( includeFieldName && type() != EOO )
            s << fieldName() << ": ";
        switch ( type() ) {
        case EOO:
            s << "EOO";
            break;
        case mongo::Date:
            s << "new Date(" << (long long) date() << ')';
            break;
        case RegEx: {
            s << "/" << regex() << '/';
            const char *p = regexFlags();
            if ( p ) s << p;
        }
        break;
        case NumberDouble:
            s.appendDoubleNice( number() );
            break;
        case NumberLong:
            s << _numberLong();
            break;
        case NumberInt:
            s << _numberInt();
            break;
        case mongo::Bool:
            s << ( boolean() ? "true" : "false" );
            break;
        case Object:
            embeddedObject().toString(s, false, full, depth+1);
            break;
        case mongo::Array:
            embeddedObject().toString(s, true, full, depth+1);
            break;
        case Undefined:
            s << "undefined";
            break;
        case jstNULL:
            s << "null";
            break;
        case MaxKey:
            s << "MaxKey";
            break;
        case MinKey:
            s << "MinKey";
            break;
        case CodeWScope:
            s << "CodeWScope( "
              << codeWScopeCode() << ", " << codeWScopeObject().toString(false, full) << ")";
            break;
        case Code:
            if ( !full &&  valuestrsize() > 80 ) {
                s.write(valuestr(), 70);
                s << "...";
            }
            else {
                s.write(valuestr(), valuestrsize()-1);
            }
            break;
        case Symbol:
        case mongo::String:
            s << '"';
            if ( !full &&  valuestrsize() > 160 ) {
                s.write(valuestr(), 150);
                s << "...\"";
            }
            else {
                s.write(valuestr(), valuestrsize()-1);
                s << '"';
            }
            break;
        case DBRef:
            s << "DBRef('" << valuestr() << "',";
            {
                mongo::OID *x = (mongo::OID *) (valuestr() + valuestrsize());
                s << *x << ')';
            }
            break;
        case jstOID:
            s << "ObjectId('";
            s << __oid() << "')";
            break;
        case BinData:
            s << "BinData";
            if (full) {
                int len;
                const char* data = binDataClean(len);
                s << '(' << binDataType() << ", " << toHex(data, len) << ')';
            }
            break;
        case Timestamp:
            s << "Timestamp " << timestampTime() << "|" << timestampInc();
            break;
        default:
            s << "?type=" << type();
            break;
        }
    }

inline BSONObj BSONElement::Obj() const { return embeddedObjectUserCheck(); }

inline BSONElement BSONElement::operator[] (const std::string& field) const {
    BSONObj o = Obj();
    return o[field];
}

inline std::ostream& operator<<( std::ostream &s, const BSONElement &e ) {
    return s << e.toString();
}

inline StringBuilder& operator<<( StringBuilder &s, const BSONElement &e ) {
    e.toString( s );
    return s;
}


    inline void BSONElement::Val(BSONObj& v) const { v = Obj(); }

    template<typename T>
    inline BSONFieldValue<BSONObj> BSONField<T>::query( const char * q , const T& t ) const {
        BSONObjBuilder b;
        b.append( q , t );
        return BSONFieldValue<BSONObj>( _name , b.obj() );
    }

    // used by jsonString()
    inline std::string escape( std::string s , bool escape_slash=false) {
        StringBuilder ret;
        for ( std::string::iterator i = s.begin(); i != s.end(); ++i ) {
            switch ( *i ) {
            case '"':
                ret << "\\\"";
                break;
            case '\\':
                ret << "\\\\";
                break;
            case '/':
                ret << (escape_slash ? "\\/" : "/");
                break;
            case '\b':
                ret << "\\b";
                break;
            case '\f':
                ret << "\\f";
                break;
            case '\n':
                ret << "\\n";
                break;
            case '\r':
                ret << "\\r";
                break;
            case '\t':
                ret << "\\t";
                break;
            default:
                if ( *i >= 0 && *i <= 0x1f ) {
                    //TODO: these should be utf16 code-units not bytes
                    char c = *i;
                    ret << "\\u00" << toHexLower(&c, 1);
                }
                else {
                    ret << *i;
                }
            }
        }
        return ret.str();
    }
    inline OpTime BSONElement::_opTime() const {
        if( type() == mongo::Date || type() == Timestamp )
            return OpTime( *reinterpret_cast< const unsigned long long* >( value() ) );
        return OpTime();
    }

    inline std::string BSONElement::_asCode() const {
        switch( type() ) {
        case mongo::String:
        case Code:
            return std::string(valuestr(), valuestrsize()-1);
        case CodeWScope:
            return std::string(codeWScopeCode(), *(int*)(valuestr())-1);
        default:
            assert(false); // 10062 ,  "not code" , 0 );
        }
        return "";
    }


} // namespace

#endif // BSON_ELEMENT_IPP
