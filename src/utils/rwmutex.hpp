/*
FalconDB, a database
Copyright (C) 2012 Maciej Gajewski <maciej.gajewski0 at gmail dot com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef FALCONDB_RWMUTEX_HPP
#define FALCONDB_RWMUTEX_HPP

#include "utils/tscclock.hpp"

#include <boost/thread/shared_mutex.hpp>

#include <atomic>
#include <cstdint>

namespace falcondb {

#ifndef NO_MUTEX_STATS
struct rwmutex_stats
{
    std::atomic<std::uint32_t> read_locks;
    std::atomic<std::uint32_t> wrtie_logs;
    std::atomic<std::uint32_t> failed_read_tries;
    std::atomic<std::uint32_t> failed_wrtie_tries;
    std::atomic<std::uint64_t> cycles_waiting_for_wrtie;
    std::atomic<std::uint64_t> cycles_waiting_for_read;
};

extern rwmutex_stats global_rwmutex_stats;
#endif // NO_MUTEX_STATS


/// Reader-writer mutex.
// Extends boost::shared_mutex by adding some useful statistics
class rwmutex
{
public:

    void lock_shared()
    {
        #ifndef NO_MUTEX_STATS
        std::uint64_t before = tsc_read();
        #endif

        _mutex.lock_shared();

        #ifndef NO_MUTEX_STATS
        global_rwmutex_stats.cycles_waiting_for_read += (tsc_read() - before);
        #endif
    }

    bool try_lock_shared()
    {
        bool r = _mutex.try_lock_shared();

        #ifndef NO_MUTEX_STATS
        global_rwmutex_stats.failed_read_tries += !r;
        #endif

        return r;
    }

    void unlock_shared()
    {
        _mutex.unlock_shared();
    }

    void lock()
    {
        #ifndef NO_MUTEX_STATS
        std::uint64_t before = tsc_read();
        #endif

        _mutex.lock();

        #ifndef NO_MUTEX_STATS
        global_rwmutex_stats.cycles_waiting_for_wrtie += (tsc_read() - before);
        #endif
    }

    bool try_lock()
    {
        bool r = _mutex.try_lock();

        #ifndef NO_MUTEX_STATS
        global_rwmutex_stats.failed_wrtie_tries += !r;
        #endif

        return r;
    }

    void unlock()
    {
        _mutex.unlock();
    }

    // RIIA scoped locks

    class scoped_write_lock
    {
    public:
        scoped_write_lock(rwmutex& m) : _m(m) { _m.lock(); }
        ~scoped_write_lock() { _m.unlock(); }
    private:
        rwmutex& _m;
    };

    class scoped_read_lock
    {
    public:
        scoped_read_lock(rwmutex& m) : _m(m) { _m.lock_shared(); }
        ~scoped_read_lock() { _m.unlock_shared(); }
    private:
        rwmutex& _m;
    };

private:

    boost::shared_mutex _mutex;
};

}

#endif
