/***************************************************************************//*! \file liprovider/sf_locks.cpp
** <L5_PRIVATE L5_SOURCE>
** \author  OktetLabs
**  \brief  CIM Provider
**   \date  2018/01/30
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

#include "sf_locks.h"
#include "sf_core.h"

namespace solarflare
{
    SharedLock::SharedLock() : counter(0)
    {
        int rc;

        rc = pthread_key_create(&key, free);
        if (rc != 0)
            THROW_PROVIDER_EXCEPTION_FMT("pthread_key_create() returned %d",
                                         rc);
    }

    void SharedLock::lock_shared()
    {
        unsigned int *ptr = NULL;
        int           rc;

        ptr = (unsigned int *)pthread_getspecific(key);
        if (ptr == NULL)
        {
            ptr = new unsigned int;
            *ptr = 0;
            rc = pthread_setspecific(key, ptr);
            if (rc != 0)
                THROW_PROVIDER_EXCEPTION_FMT(
                            "pthread_setspecific() returned %d", rc);
        }

        if (*ptr > 0xffff)
            THROW_PROVIDER_EXCEPTION_FMT("Too big per-thread "
                                         "lock counter value");

        if (counter > 0xffff)
            THROW_PROVIDER_EXCEPTION_FMT("Too big global lock counter");

        if (*ptr == 0)
        {
            mutex_shared.lock();

            mutex.lock();
            counter++;
            mutex.unlock();

            mutex_shared.unlock();
        }

        (*ptr)++;
    }

    void SharedLock::unlock_shared()
    {
        unsigned int *ptr = NULL;
        int           rc;

        if (counter == 0)
            THROW_PROVIDER_EXCEPTION_FMT("Global lock counter is zero, "
                                         "but unlock_shared() is called");

        ptr = (unsigned int *)pthread_getspecific(key);
        if (ptr == NULL)
            THROW_PROVIDER_EXCEPTION_FMT("unlock_shared() called "
                                         "while there is no per-thread "
                                         "lock counter");

        if (*ptr == 0)
            THROW_PROVIDER_EXCEPTION_FMT("unlock_shared() called "
                                         "while per-thread lock counter "
                                         "is zero");

        (*ptr)--;

        if (*ptr == 0)
        {
            mutex.lock();
            counter--;
            if (counter <= 1)
                cond.signal();
            mutex.unlock();
        }
    }

    void SharedLock::lock_exclusive()
    {
        lock_shared();

        // This will prevent other threads
        // from acquiring lock.
        mutex_shared.lock();

        // Wait until other threads release
        // their locks.

        mutex.lock();

        while (counter > 1)
            cond.wait(mutex);

        mutex.unlock();
    }

    void SharedLock::unlock_exclusive()
    {
        mutex_shared.unlock();
        unlock_shared();
    }

    AutoSharedLock::AutoSharedLock(SharedLock &lock, bool exclusive) :
        slock(lock), excl(exclusive)
    {
        if (excl)
            slock.lock_exclusive();
        else
            slock.lock_shared();
    }

    AutoSharedLock::~AutoSharedLock()
    {
        if (excl)
            slock.unlock_exclusive();
        else
            slock.unlock_shared();
    }
}
