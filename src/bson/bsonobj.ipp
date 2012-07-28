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
#ifndef BSON_OBJ_IPP
#define BSON_OBJ_IPP

#include <map>
#include <limits>
#include <cassert>
#include "bson/bsonobjiterator.hpp"
#include "bson/bsonobjbuilder.hpp"

#if defined(_WIN32)
#undef max
#undef min
#endif

namespace mongo {


// deep (full) equality
inline bool BSONObj::equal(const BSONObj &rhs) const {
    BSONObjIterator i(*this);
    BSONObjIterator j(rhs);
    BSONElement l,r;
    do {
        // so far, equal...
        l = i.next();
        r = j.next();
        if ( l.eoo() )
            return r.eoo();
    } while( l == r );
    return false;
}

inline void BSONObj::_assertInvalid() const {
    StringBuilder ss;
    int os = objsize();
    ss << "Invalid BSONObj size: " << os << " (0x" << toHex( &os, 4 ) << ')';
    try {
        BSONElement e = firstElement();
        ss << " first element: " << e.toString();
    }
    catch ( ... ) { }
    assert( 0 );
}

inline BSONObj BSONObj::copy() const {
    Holder *h = (Holder*) malloc(objsize() + sizeof(unsigned));
    h->zero();
    memcpy(h->data, objdata(), objsize());
    return BSONObj(h);
}

inline BSONObj BSONObj::getOwned() const {
    if ( isOwned() )
        return *this;
    return copy();
}

inline void BSONObj::getFields(unsigned n, const char **fieldNames, BSONElement *fields) const {
    BSONObjIterator i(*this);
    while ( i.more() ) {
        BSONElement e = i.next();
        const char *p = e.fieldName();
        for( unsigned i = 0; i < n; i++ ) {
            if( strcmp(p, fieldNames[i]) == 0 ) {
                fields[i] = e;
                break;
            }
        }
    }
}

inline BSONElement BSONObj::getField(const StringData& name) const {
    BSONObjIterator i(*this);
    while ( i.more() ) {
        BSONElement e = i.next();
        if ( strcmp(e.fieldName(), name.data()) == 0 )
            return e;
    }
    return BSONElement();
}

inline int BSONObj::getIntField(const char *name) const {
    BSONElement e = getField(name);
    return e.isNumber() ? (int) e.number() : std::numeric_limits< int >::min();
}

inline bool BSONObj::getBoolField(const char *name) const {
    BSONElement e = getField(name);
    return e.type() == Bool ? e.boolean() : false;
}

inline const char * BSONObj::getStringField(const char *name) const {
    BSONElement e = getField(name);
    return e.type() == String ? e.valuestr() : "";
}


inline bool BSONObj::isValid() const {
    int x = objsize();
    return x > 0;
}

inline bool BSONObj::getObjectID(BSONElement& e) const {
    BSONElement f = getField("_id");
    if( !f.eoo() ) {
        e = f;
        return true;
    }
    return false;
}

// {a: {b:1}} -> {a.b:1}
void nested2dotted(BSONObjBuilder& b, const BSONObj& obj, const std::string& base="");
inline BSONObj nested2dotted(const BSONObj& obj) {
    BSONObjBuilder b;
    nested2dotted(b, obj);
    return b.obj();
}

// {a.b:1} -> {a: {b:1}}
void dotted2nested(BSONObjBuilder& b, const BSONObj& obj);
inline BSONObj dotted2nested(const BSONObj& obj) {
    BSONObjBuilder b;
    dotted2nested(b, obj);
    return b.obj();
}

inline std::string BSONObj::toString( bool isArray, bool full ) const {
    if ( isEmpty() ) return "{}";
    StringBuilder s;
    toString(s, isArray, full);
    return s.str();
}

inline void BSONObj::toString( StringBuilder& s,  bool isArray, bool full, int depth ) const {
    if ( isEmpty() ) {
        s << "{}";
        return;
    }

    s << ( isArray ? "[ " : "{ " );
    BSONObjIterator i(*this);
    bool first = true;
    while ( 1 ) {
        assert( i.moreWithEOO() );
        BSONElement e = i.next( true );
        assert( e.size() > 0 );
        assert( e.size() < ( 1 << 30 ) );
        int offset = (int) (e.rawdata() - this->objdata());
        assert( e.size() + offset <= this->objsize() );
        e.validate();
        bool end = ( e.size() + offset == this->objsize() );
        if ( e.eoo() ) {
            assert(end );
            break;
        }
        if ( first )
            first = false;
        else
            s << ", ";
        e.toString( s, !isArray, full, depth );
    }
    s << ( isArray ? " ]" : " }" );
}

/* return has eoo() true if no match
       supports "." notation to reach into embedded objects
    */
inline BSONElement BSONObj::getFieldDotted(const char *name) const {
    BSONElement e = getField( name );
    if ( e.eoo() ) {
        const char *p = strchr(name, '.');
        if ( p ) {
            std::string left(name, p-name);
            BSONObj sub = getObjectField(left.c_str());
            return sub.isEmpty() ? BSONElement() : sub.getFieldDotted(p+1);
        }
    }

    return e;
}

inline BSONObj BSONObj::getObjectField(const char *name) const {
    BSONElement e = getField(name);
    BSONType t = e.type();
    return t == Object || t == Array ? e.embeddedObject() : BSONObj();
}

inline int BSONObj::nFields() const {
    int n = 0;
    BSONObjIterator i(*this);
    while ( i.moreWithEOO() ) {
        BSONElement e = i.next();
        if ( e.eoo() )
            break;
        n++;
    }
    return n;
}

inline BSONObj::BSONObj() {
    /* little endian ordering here, but perhaps that is ok regardless as BSON is spec'd
           to be little endian external to the system. (i.e. the rest of the implementation of bson,
           not this part, fails to support big endian)
        */
    static char p[] = { /*size*/5, 0, 0, 0, /*eoo*/0 };
    _objdata = p;
}


inline void BSONObj::elems(std::vector<BSONElement> &v) const {
    BSONObjIterator i(*this);
    while( i.more() )
        v.push_back(i.next());
}

inline void BSONObj::elems(std::list<BSONElement> &v) const {
    BSONObjIterator i(*this);
    while( i.more() )
        v.push_back(i.next());
}

template <class T>
void BSONObj::Vals(std::vector<T>& v) const {
    BSONObjIterator i(*this);
    while( i.more() ) {
        T t;
        i.next().Val(t);
        v.push_back(t);
    }
}
template <class T>
void BSONObj::Vals(std::list<T>& v) const {
    BSONObjIterator i(*this);
    while( i.more() ) {
        T t;
        i.next().Val(t);
        v.push_back(t);
    }
}

template <class T>
void BSONObj::vals(std::vector<T>& v) const {
    BSONObjIterator i(*this);
    while( i.more() ) {
        try {
            T t;
            i.next().Val(t);
            v.push_back(t);
        }
        catch(...) { }
    }
}
template <class T>
void BSONObj::vals(std::list<T>& v) const {
    BSONObjIterator i(*this);
    while( i.more() ) {
        try {
            T t;
            i.next().Val(t);
            v.push_back(t);
        }
        catch(...) { }
    }
}

inline std::ostream& operator<<( std::ostream &s, const BSONObj &o ) {
    return s << o.toString();
}


inline StringBuilder& operator<<( StringBuilder &s, const BSONObj &o ) {
    o.toString( s );
    return s;
}

inline std::string BSONObj::hexDump() const {
    std::stringstream ss;
    const char *d = objdata();
    int size = objsize();
    for( int i = 0; i < size; ++i ) {
        ss.width( 2 );
        ss.fill( '0' );
        ss << std::hex << (unsigned)(unsigned char)( d[ i ] ) << std::dec;
        if ( ( d[ i ] >= '0' && d[ i ] <= '9' ) || ( d[ i ] >= 'A' && d[ i ] <= 'z' ) )
            ss << '\'' << d[ i ] << '\'';
        if ( i != size - 1 )
            ss << ' ';
    }
    return ss.str();
}


inline BSONObj BSONObj::removeField(const StringData& name) const {
    BSONObjBuilder b;
    BSONObjIterator i(*this);
    while ( i.more() ) {
        BSONElement e = i.next();
        const char *fname = e.fieldName();
        if( strcmp(name.data(), fname) )
            b.append(e);
    }
    return b.obj();
}

typedef std::map<std::string, BSONElement> BSONMap;
inline BSONMap bson2map(const BSONObj& obj) {
    BSONMap m;
    BSONObjIterator it(obj);
    while (it.more()) {
        BSONElement e = it.next();
        m[e.fieldName()] = e;
    }
    return m;
}

} // namespace

#endif // BSON_OBJ_IPP
