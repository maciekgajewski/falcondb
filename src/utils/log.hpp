#ifndef FALCONDB_LOG_HPP
#define FALCONDB_LOG_HPP

#include "utils/string.hpp"

#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <thread>

namespace falcondb { namespace logging {

namespace interface {

class logger
{
};

}

enum class level
{
    error = 0,
    warn,
    spam,
    debug,
    info
};

inline
std::ostream& operator << (std::ostream& out, level l)
{
    switch(l) {
        case level::error: return out << "ERROR";
        case level::warn:  return out << "WARN ";
        case level::spam:  return out << "SPAM ";
        case level::debug: return out << "DEBUG";
        case level::info:  return out << "INFO ";
    }
    return out;
}

template <typename ...T>
inline
void log(level l, T... v)
{
    const boost::posix_time::ptime now=
          boost::posix_time::microsec_clock::local_time();

    std::cerr
        << now
        << " (" << std::this_thread::get_id() << ") "
        << l << ' '
        << build_string(v...)
        << std::endl;
}

template <typename ...T> inline void info(T... v) { log(level::info, v...); }
template <typename ...T> inline void debug(T... v) { log(level::debug, v...); }
template <typename ...T> inline void error(T... v) { log(level::error, v...); }

}

}

#endif

