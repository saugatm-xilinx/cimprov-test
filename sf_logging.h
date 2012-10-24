#ifndef SOLARFLARE_SF_LOGGING_H
#define SOLARFLARE_SF_LOGGING_H 1

#include <cimple/cimple.h>
#include <cstdarg>

// Logging facility

namespace solarflare 
{
    using cimple::uint64;
    using cimple::Mutex;
    using cimple::String;
    using cimple::Datetime;
    using cimple::Buffer;
    
    /// @brief Log severity levels
    enum LogLevel {
        LogFatal,
        LogError,
        LogWarning,
        LogInfo,
        LogDebug
    };

    /// @brief Log entry class
    class LogEntry {
        uint64 serial; //< Serial number of the message
        Datetime timestamp; //< Posting timestamp 
        String messageStr; //< Log message 
        const char *locFile; //< Originating source file
        int locLine; //< Originating source line
    public:
        /// Constructor
        ///
        /// @param no     Serial number
        /// @param stamp  Timestamp
        /// @param msg    Log message
        /// @param fname  Origin file name
        /// @param lno    Origin line number
        LogEntry(uint64 no, const Datetime& stamp, const String& msg, 
                 const char *fname = NULL, int lno = 0) :
            serial(no), timestamp(stamp), messageStr(msg), 
            locFile(fname), locLine(lno) {}
        /// Empty constructor
        LogEntry() : serial(0), locFile(NULL), locLine(0) {}
        /// @return unique id of the entry (serial number)
        uint64 id() const { return serial; };
        /// sets the serial number to @a sno
        void id(uint64 sno) { serial = sno; }
        /// @return timestamp of the message
        const Datetime& stamp() const { return timestamp; }
        /// @return message string
        const String& message() const { return messageStr; }
        /// @return origin filename
        const char *file() const { return locFile; }
        /// @return origin line number
        int line() const { return locLine; }
        /// Print a log entry at level @p using CIMPLE logging
        void printLog(LogLevel at) const;
    };

    /// Abstract iterator of LogEntry instances
    class LogEntryIterator {
    public:
        virtual bool process(const LogEntry& le) = 0;
    };

    class Logger 
    {
        bool enabled;
        LogLevel level;
        unsigned size;
        mutable Mutex lock;
        Buffer formatter;
        LogEntry *entries;
        uint64 serial;
        const char *descr;
        String doFormat(const char *fmt, va_list args);
    public:
        /// Constructor
        ///
        /// @param lvl   Severity level
        /// @param sz    Size of the message queue
        /// @param d     Log description
        Logger(LogLevel lvl, unsigned sz, const char *d) :
            enabled(true), level(lvl), size(sz), 
            entries(new LogEntry[sz]), serial(0), descr(d) {}
        ~Logger() { delete[] entries; }

        /// Puts a copy of a prepared LogEntry into the queue
        void put(const LogEntry& e);

        /// Logs a simple string message
        void log(const String& s)
        {
            put(LogEntry(0, Datetime::now(), s));
        }

        /// Format and log a printf-style message
        void format(const char *fmt, ...);

        /// Applies `iter' to all the log records inside this log
        bool forAllEntries(LogEntryIterator& iter) const;

        /// @return Log description
        const char *description() const { return descr; }

        /// @return Log severity
        LogLevel severity() const { return level; }

        /// @return Is logging actually enabled
        bool isEnabled() const { return enabled; }

        /// Enable/disabled logging via this logger
        void enable(bool en) { enabled = en; }

        /// Delete all log entries in the memory
        /// @note On-disk CIMPLE log file is unaffected, if present
        void clear();

        /// @return maximum number of log entries at once
        unsigned logSize() const { return size; }

        /// @return current number of log entries
        unsigned currentSize() const;

        /// Logger instance for errors
        static Logger errorLog;

        /// Logger instance for system events
        static Logger eventLog;

        /// Logger instance for debug messages
        static Logger debugLog;

        /// List of all known loggers
        static Logger *const knownLogs[];
    };
} // namespace

#endif   // SOLARFLARE_SF_CORE_H
