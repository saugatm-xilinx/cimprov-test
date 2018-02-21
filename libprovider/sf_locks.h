/***************************************************************************//*! \file liprovider/sf_locks.h
** <L5_PRIVATE L5_SOURCE>
** \author  OktetLabs
**  \brief  CIM Provider
**   \date  2018/01/30
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

#ifndef SOLARFLARE_SF_LOCKS_H
#define SOLARFLARE_SF_LOCKS_H 1

#include <pthread.h>
#include <cimple/Mutex.h>
#include <cimple/Cond.h>

namespace solarflare
{
    using cimple::Mutex;
    using cimple::Cond;

    ///
    /// Shared lock. Any number of threads can acquire
    /// this lock in a shared mode, unless some thread has
    /// acquired this lock in an exclusive mode. While a
    /// thread has this lock in an exclusive mode, no other
    /// thread can have this lock in any mode.
    /// This lock is recursive, the same thread can acquire
    /// it multiple times in any mode.
    ///
    /// May be pthread RW lock could have been used instead,
    /// however related pthread calls are not mentioned among
    /// supported ones in CIM Provider Development Guide for
    /// ESXi 6.0.
    class SharedLock {
        pthread_key_t key;    ///< Key used to store thread-specific
                              ///  lock counter
        Cond cond;            ///< Condition variable used to wait
                              ///  until other threads release locks

        unsigned int counter; ///< How many threads acquired this lock
        Mutex mutex;          ///< Mutex protecting counter
        Mutex mutex_shared;   ///< Mutex which should be acquired before
                              ///  incrementing counter

    public:
        ///
        /// Constructor.
        SharedLock();

        ///
        /// Acquire lock in a shared mode.
        void lock_shared();

        ///
        /// Release lock acquired in a shared mode.
        void unlock_shared();

        ///
        /// Acquire lock in an exclusive mode.
        void lock_exclusive();

        ///
        /// Release lock acquired in an exclusive mode.
        void unlock_exclusive();
    };

    ///
    /// Class used to acquire SharedLock automatically
    /// when an object of this type is defined, and release
    /// it when it is destroyed.
    class AutoSharedLock {
        SharedLock &slock;  ///< Shared lock
        bool        excl;   ///< If true, acquire lock in an
                            ///  exclusive mode

    public:
        ///
        /// Constructor.
        ///
        /// @param lock       Lock to acquire.
        /// @param exclusive  Whether to use exclusive mode.
        AutoSharedLock(SharedLock &lock, bool exclusive);

        ///
        /// Destructor.
        ~AutoSharedLock();
    };
}

#endif // SOLARFLARE_SF_LOCKS_H
