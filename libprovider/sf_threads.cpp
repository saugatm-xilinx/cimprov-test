#include "sf_threads.h"

#if defined(linux)
#include <unistd.h>
#endif

namespace solarflare 
{
    using cimple::Log_Call_Frame;
    using cimple::_log_enabled_state;
    using cimple::LL_DBG;
    using cimple::Auto_Mutex;

    void *Thread::doThread(void *self)
    {
        bool status;
        Thread *object = static_cast<Thread *>(self);

        status = object->threadProc();

        object->stateLock.lock();
        if (object->state == Aborting)
            object->state = Aborted;
        else
            object->state = status ? Succeeded : Failed;
        object->stateLock.unlock();
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
    
    void Thread::start()
    {
        Auto_Mutex autolock(stateLock);
        if (state == Running || state == Aborting)
            return;
        state = Running;
        startedAt = cimple::Datetime::now();
        if (!Thread::create_detached(*this, doThread, this))
            state = Aborted;
    }

    void Thread::stop()
    {
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
