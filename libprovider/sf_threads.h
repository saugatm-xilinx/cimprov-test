#ifndef SOLARFLARE_SF_THREADS_H
#define SOLARFLARE_SF_THREADS_H 1

#include <cimple/cimple.h>
#include <cimple/Buffer.h>
#include <vector>

// !!! OS-specific !!!

namespace solarflare 
{
    // fixme: what do we actually need here?
    using cimple::Buffer;
    using cimple::String;
    using cimple::Datetime;
    using cimple::uint64;
    using cimple::Mutex;
    using std::vector;

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

        /// List with saved thread instances
        static vector<Thread *> threads;
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
        /// Unique ID of thread
        String id;

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

        /// Function which duplicates thread data for long-lived threads list
        virtual Thread* dup() const = 0;
    public:
        /// Constructors.
        Thread() : state(NotRun), id("") {}
        Thread(String sid) : state(NotRun), id(sid) {}

        /// Find thread in threads list
        Thread* find() const;

        /// Find and update thread instance in threads list,
        /// if it doesn't exist add current instance to the list
        Thread* findUpdate();

        /// Calls threadProc() from the forThread() action handler.
        /// We can't use threadProc() directly, because some upper
        /// objects from the main thread could be dead already, so we need
        /// to re-create it.
        void asyncHandler();

        /// @return thread ID retrieved from the corresponding CIM instance
        virtual String getThreadID() const = 0;

        /// Copy data from the current thread instance to tempThr instance
        virtual void update(Thread *tempThr) = 0;
        
        /// @return ID of the thread
        String threadID() const { return id; }

        /// @return current thread run state
        State currentState() const;

        /// @return start time of the thread
        Datetime startTime() const;

        /// Makes the thread running. The state is set to Running.
        /// If we run asynchroniosly, call findUpdate() to
        /// save thread data and call start() on corresponding
        /// instance from the threads list.
        void start(bool sync = false);

        /// Attempts to stop the thread. It is not guaranteed that this
        /// method actually has any effect; caller must poll currentState()
        /// for state change.  As there is no cross-platform universal way
        /// to stop threads this needs co-operation from threadProc() with
        /// the help of terminate()
        ///
        /// Thread state is set to Aborting. If termination goes fast enough
        /// the state may be already set to Aborted.
        ///
        /// If we run asynchroniosly, call findUpdate() to
        /// save thread data and call stop() on corresponding
        /// instance from the threads list.
        void stop(bool sync = false);

        /// The default implementation depends on the current state and
        /// conforms to Job Control Profile 7.1.3.
        ///
        /// @return Percentage of task completion.
        virtual unsigned percentage() const;
    };

} // namespace

#endif  // SOLARFLARE_SF_THREADS_H
