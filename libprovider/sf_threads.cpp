/***************************************************************************//*! \file liprovider/sf_threads.cpp
** <L5_PRIVATE L5_SOURCE>
** \author  OktetLabs
**  \brief  CIM Provider
**   \date  2013/10/02
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

#include "sf_threads.h"
#include "sf_provider.h"
#include "sf_logging.h"

#if defined(linux)
#include <unistd.h>
#endif

namespace solarflare
{
    using cimple::Log_Call_Frame;
    using cimple::_log_enabled_state;
    using cimple::LL_DBG;
    using cimple::LL_ERR;
    using cimple::Auto_Mutex;
    using cimple::SF_ConcreteJob;
    using cimple::Array;

    Array<Thread *> Thread::threads;

    Thread *Thread::find() const
    {
        int i;
        String tid;

        if (*id.c_str() == '\0')
            tid = getThreadID();
        else
            tid = id;

        for (i = 0; i < (int)threads.size(); i++)
        {
            if (0 == strcmp(tid.c_str(),
                        threads[i]->threadID().c_str()))
                return threads[i];
        }

        return NULL;
    }

    Thread *Thread::findUpdate()
    {
        int i;

        if (*id.c_str() == '\0')
            id = getThreadID();

        for (i = 0; i < (int)threads.size(); i++)
        {
            if (0 == strcmp(id.c_str(),
                        threads[i]->threadID().c_str()))
            {
                threads[i]->update(this);
                return threads[i];
            }
        }

        Thread *thr = dup();
        threads.append(thr);
        return thr;
    }

    void Thread::asyncHandler()
    {
        bool status = threadProc();

        stateLock.lock();
        finishedAt = cimple::Datetime::now();
        if (state == Aborting)
            state = Aborted;
        else
            state = status ? Succeeded : Failed;
        stateLock.unlock();
    }

    void AsyncRunner::handler(SystemElement &se,
                              unsigned)
    {
        Thread *thr = se.embeddedThread();
        if (thr != NULL)
            thr->asyncHandler();
    }

    void *Thread::doThread(void *self)
    {
        char *name = (char *)self;

        cimple::SF_ConcreteJob *job = cimple::SF_ConcreteJob::create(true);
        job->InstanceID.set(name);

        AsyncRunner runner(job);
        if (!runner.forThread())
            PROVIDER_LOG_ERR("Failed to find Thread");

        exit(NULL);
        return NULL;
    }

    Thread::State Thread::currentState() const
    {
        State s;

        stateLock.lock();
        s = state;
        stateLock.unlock();
        return s;
    }

    Datetime Thread::startTime() const
    {
        Datetime t;

        stateLock.lock();
        t = startedAt;
        stateLock.unlock();
        return t;
    }

    Datetime Thread::finishTime() const
    {
        Datetime t;

        stateLock.lock();
        t = finishedAt;
        stateLock.unlock();
        return t;
    }

    void Thread::start(bool sync)
    {
        if (!sync)
        {
            Thread *savedThr = findUpdate();
            savedThr->start(true);
            return;
        }

        Auto_Mutex autolock(stateLock);

        if (state == Running || state == Aborting)
            return;
        state = Running;
        startedAt = cimple::Datetime::now();
        if (Thread::create_detached(*this, doThread, (void *)threadID().c_str()))
        {
            finishedAt = startedAt;
            state = Aborted;
        }
    }

    void Thread::stop(bool sync)
    {
        if (!sync)
        {
            Thread *savedThr = findUpdate();
            savedThr->stop(true);
            return;
        }

        Auto_Mutex autolock(stateLock);

        if (state != Running)
            return;
        state = Aborting;
        terminate();
    }

    unsigned Thread::percentage() const
    {
        switch (currentState())
        {
            case Running:
                return 50;
            case Succeeded:
                return 100;
            default:
                return 0;
        }
    }
} // namespace
