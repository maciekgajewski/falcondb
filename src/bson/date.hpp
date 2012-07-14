#pragma once
#ifndef MONGO_DATE_HPP
#define MONGO_DATE_HPP

#include "bson/misc.hpp"

#include <cstdint>
#include <cassert>
#include <ctime>
#include <limits>

namespace mongo {

struct Date_t {
    // TODO: make signed (and look for related TODO's)
    uint64_t millis;
    Date_t(): millis(0) {}
    Date_t(uint64_t m): millis(m) {}
    operator uint64_t&() { return millis; }
    operator const uint64_t&() const { return millis; }
    void toTm (tm *buf) {
        time_t dtime = toTimeT();
#if defined(_WIN32)
        gmtime_s(buf, &dtime);
#else
        gmtime_r(&dtime, buf);
#endif
    }
    std::string toString() const {
        char buf[64];
        time_t_to_String(toTimeT(), buf);
        return buf;
    }
    time_t toTimeT() const {
        // cant use uassert from bson/util
        assert((int64_t)millis >= 0); // TODO when millis is signed, delete
        assert(((int64_t)millis/1000) < (std::numeric_limits<time_t>::max)());
        return millis / 1000;
    }
};

}


#endif // MONGO_DATE_HPP
