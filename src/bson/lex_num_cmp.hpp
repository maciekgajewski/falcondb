/*
 *    Copyright (C) 2012 Maciej Gajewski <maciej.gajewski0 at gmail dot com>
 *    Basend on mongodb code, Copyright 2009 10gen Inc.
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
#ifndef BSON_LEX_NUM_CMP_HPP
#define BSON_LEX_NUM_CMP_HPP

#include <string>
#include <cctype>
#include <cstring>

namespace bson
{
/** Functor for combining lexical and numeric comparisons. */
class lex_num_cmp {
public:
    /** @param lex_only - compare all characters lexically, including digits. */
    lex_num_cmp( bool lex_only ) : _lex_only( lex_only ) { }
    /**
     * Non numeric characters are compared lexicographically; numeric substrings
     * are compared numerically; dots separate ordered comparable subunits.
     * For convenience, character 255 is greater than anything else.
     * @param lex_only - compare all characters lexically, including digits.
     */
    static int cmp( const char *s1, const char *s2, bool lex_only )
    {
        bool startWord = true;

        while( *s1 && *s2 ) {

            bool d1 = ( *s1 == '.' );
            bool d2 = ( *s2 == '.' );
            if ( d1 && !d2 )
                return -1;
            if ( d2 && !d1 )
                return 1;
            if ( d1 && d2 ) {
                ++s1; ++s2;
                startWord = true;
                continue;
            }

            bool p1 = ( *s1 == (char)255 );
            bool p2 = ( *s2 == (char)255 );
            //cout << "\t\t " << p1 << "\t" << p2 << endl;
            if ( p1 && !p2 )
                return 1;
            if ( p2 && !p1 )
                return -1;

            if ( !lex_only ) {

                bool n1 = std::isdigit( *s1 );
                bool n2 = std::isdigit( *s2 );

                if ( n1 && n2 ) {
                    // get rid of leading 0s
                    if ( startWord ) {
                        while ( *s1 == '0' ) s1++;
                        while ( *s2 == '0' ) s2++;
                    }

                    char * e1 = (char*)s1;
                    char * e2 = (char*)s2;

                    // find length
                    // if end of string, will break immediately ('\0')
                    while ( std::isdigit (*e1) ) e1++;
                    while ( std::isdigit (*e2) ) e2++;

                    int len1 = (int)(e1-s1);
                    int len2 = (int)(e2-s2);

                    int result;
                    // if one is longer than the other, return
                    if ( len1 > len2 ) {
                        return 1;
                    }
                    else if ( len2 > len1 ) {
                        return -1;
                    }
                    // if the lengths are equal, just strcmp
                    else if ( (result = std::strncmp(s1, s2, len1)) != 0 ) {
                        return result;
                    }

                    // otherwise, the numbers are equal
                    s1 = e1;
                    s2 = e2;
                    startWord = false;
                    continue;
                }

                if ( n1 )
                    return 1;

                if ( n2 )
                    return -1;
            }

            if ( *s1 > *s2 )
                return 1;

            if ( *s2 > *s1 )
                return -1;

            s1++; s2++;
            startWord = false;
        }

        if ( *s1 )
            return 1;
        if ( *s2 )
            return -1;
        return 0;
    }

    int cmp( const char *s1, const char *s2 ) const
    {
        return cmp( s1, s2, _lex_only );
    }

    bool operator()( const char *s1, const char *s2 ) const
    {
        return cmp( s1, s2 ) < 0;
    }
    bool operator()( const std::string &s1, const std::string &s2 ) const
    {
        return (*this)( s1.c_str(), s2.c_str() );
    }
private:
    bool _lex_only;
};

}

#endif // BSON_LEX_NUM_CMP_HPP
