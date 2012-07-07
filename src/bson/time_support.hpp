// @file time_support.h

/*    Copyright 2010 10gen Inc.
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

#include <ctime>
#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "bson/util/misc.h"  // Date_t

namespace mongo {

    void time_t_to_Struct(time_t t, struct tm * buf , bool local = false );

    // uses ISO 8601 dates without trailing Z
    // colonsOk should be false when creating filenames
    std::string terseCurrentTime(bool colonsOk=true);

    std::string timeToISOString(time_t time);

    boost::gregorian::date currentDate();

    // parses time of day in "hh:mm" format assuming 'hh' is 00-23
    bool toPointInTime( const std::string& str , boost::posix_time::ptime* timeOfDay );

    void sleepsecs(int s);
    void sleepmillis(long long ms);
    void sleepmicros(long long micros);

    // DO NOT TOUCH except for testing
    void jsTimeVirtualSkew( long long skew );

    void jsTimeVirtualThreadSkew( long long skew );
    long long getJSTimeVirtualThreadSkew();

    /** Date_t is milliseconds since epoch */
     Date_t jsTime();

    /** warning this will wrap */
    unsigned curTimeMicros();
    unsigned long long curTimeMicros64();
    unsigned long long curTimeMillis64();

    // these are so that if you use one of them compilation will fail
    char *asctime(const struct tm *tm);
    char *ctime(const time_t *timep);
    struct tm *gmtime(const time_t *timep);
    struct tm *localtime(const time_t *timep);

}  // namespace mongo
