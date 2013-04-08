#ifndef SOLARFLARE_SF_THREADS_H
#define SOLARFLARE_SF_THREADS_H 1

#include <cimple/cimple.h>
#include <cimple/Buffer.h>

// !!! OS-specific !!!

namespace solarflare 
{
    // fixme: what do we actually need here?
    using cimple::Buffer;
    using cimple::String;
    using cimple::Datetime;
    using cimple::uint64;
    using cimple::Mutex;


    /// @brief Thread abstraction. Unlike CIMPLE's own class, this class
    /// shall be subclassed, as the thread encapsulates its behaviour via
    /// threadProc() method.  The class also supports querying thread state
    /// and stopping the thread.
    class Thread : public cimple::Thread {
    public:
        /// Thread run state
        enum State {
            NotRun,    //< have never been run
            Running,   //< running now
            Succeeded, //< finished with success
            Failed,    //< finished with failure
            Aborting,  //< external termination requested but not yet completed
            Aborted,   //< externally terminated
        };

    private:
        /// State change lock
        mutable Mutex stateLock;

        /// Actual thread run state
        State state;

        /// An adaptor between cimple::Thread interface and threadProc()
        static void *doThread(void *self);

        /// Start time
        Datetime startedAt;

    protected:
        /// The actual thread routine. When Thread class is subclassed this
        /// method should be overridden to perform subclass-specific
        /// actions. I.e. diagnostic thread class should perform
        /// diagnostic-specific steps.
        ///
        /// @return true for successful termination (state := Succeeded)
        /// false for failure (state := failed)
        virtual bool threadProc() = 0;

        /// The helper to make threadProc() terminate.
        /// Shall be overriden if threadProc() in the subclass actually
        /// supports unsolicited termination.
        /// The method is always called under stateLock
        virtual void terminate() {}

    public:
        /// Constructor.
        Thread() : state(NotRun) {}

        /// @return current thread run state
        State currentState() const;

        /// @return start time of the thread
        Datetime startTime() const;

        /// Makes the thread running. The state is set to Running.
        void start();

        /// Attempts to stop the thread. It is not guaranteed that this
        /// method actually has any effect; caller must poll currentState()
        /// for state change.  As there is no cross-platform universal way
        /// to stop threads this needs co-operation from threadProc() with
        /// the help of terminate()
        ///
        /// Thread state is set to Aborting. If termination goes fast enough
        /// the state may be already set to Aborted.
        void stop();

        /// The default implementation depends on the current state and
        /// conforms to Job Control Profile 7.1.3.
        ///
        /// @return Percentage of task completion.
        virtual unsigned percentage() const;
    };

} // namespace

#endif  // SOLARFLARE_SF_THREADS_H
