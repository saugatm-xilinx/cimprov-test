/***************************************************************************//*! \file liprovider/sf_logging.h
** <L5_PRIVATE L5_SOURCE>
** \author  OktetLabs
**  \brief  CIM Provider
**   \date  2013/10/02
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

#ifndef SOLARFLARE_SF_LOGGING_H
#define SOLARFLARE_SF_LOGGING_H 1

#include <cimple/cimple.h>
#include <cstdarg>
#include <cimple/log_file_path.h>

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

    class Diagnostic;

    /// @brief Log entry class
    class LogEntry {
        LogLevel level; //< Logging level
        uint64 serial; //< Serial number of the message
        Datetime timestamp; //< Posting timestamp 
        String messageStr; //< Log message 
        unsigned errorCode; //< Error code
        unsigned nPassed; //< No of passed iterations
        unsigned nFailed; //< No of failed iterations
        String file;      //< Path to file where log is printed
        unsigned line;    //< Line number in the file
    public:
        /// Constructor
        ///
        /// @param no     Serial number
        /// @param stamp  Timestamp
        /// @param msg    Log message
        /// @param f      File where the code printing this log resides
        /// @param ln     Line number in the file
        /// @param code   Error code
        /// @param npass  No of passed iterations
        /// @param nfail  No of failed iterations
        LogEntry(uint64 no, const Datetime& stamp, const String& msg, 
                 String f = String(""), unsigned ln = 0, unsigned code = 0,
                 unsigned npass = 0, unsigned nfail = 0,
                 LogLevel lvl = LogInfo) :
            level(lvl), serial(no), timestamp(stamp), messageStr(msg), 
            errorCode(code), nPassed(npass), nFailed(nfail),
            file(f), line(ln) {}
        /// Empty constructor
        LogEntry() : level(LogInfo), serial(0), errorCode(0), nPassed(0), nFailed(0),
                     file(""), line(0) {}
        /// @return unique id of the entry (serial number)
        uint64 id() const { return serial; };
        /// sets the serial number to @a sno
        void id(uint64 sno) { serial = sno; }
        /// @return timestamp of the message
        const Datetime& stamp() const { return timestamp; }
        /// @return message string
        const String& message() const { return messageStr; }
        /// @return error code
        unsigned error() const { return errorCode; }
        /// @return No of passed iterations
        unsigned passed() const { return nPassed; }
        /// @return No of failed iterations
        unsigned failed() const { return nFailed; }
        /// @return severity level
        LogLevel severity() const { return level; }
        /// Print a log entry at level @p using CIMPLE logging
        void printLog() const;
        /// @return File where the code printing this log resides
        String getFile() const { return file; }
        /// @return Number of line in the file where the code
        ///         printing this log resides
        unsigned getLine() const { return line; }
    };

    class Logger 
    {
        bool enabled;
        LogLevel defaultLevel;
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
            enabled(true), defaultLevel(lvl), size(sz), 
            entries(new LogEntry[sz]), serial(0), descr(d) {}

        /// Copy constructor - do not copy lock!
        Logger(const Logger &src) : enabled(src.enabled),
          defaultLevel(src.defaultLevel), size(src.size),
          formatter(src.formatter),
          serial(src.serial), descr(src.descr)
        {
            unsigned int i = 0;

            entries = new LogEntry[src.size];

            for (i = 0; i < src.serial; i++)
                entries[i] = LogEntry(src.entries[i]);
        }

        ~Logger() { delete[] entries; }

        /// Puts a copy of a prepared LogEntry into the queue
        void put(const LogEntry& e);

        /// Logs a simple string message
        void log(const String& s)
        {
            put(LogEntry(0, Datetime::now(), s, "",
                         0, 0, 0, 0, defaultLevel));
        }

        /// Logs a simple string message with file and line of code
        /// printing this message included
        void log(const char *file, unsigned line, const String& s)
        {
            put(LogEntry(0, Datetime::now(), s, file,
                         line, 0, 0, 0, defaultLevel));
        }

        /// Log diagnostic status
        void logStatus(const String& s, unsigned err = 0, 
                       unsigned npass = 0, unsigned nfail = 0)
        {
            put(LogEntry(0, Datetime::now(), s, "", 0, err, npass, nfail,
                         err != 0 || nfail != 0 ? LogError : defaultLevel));
        }

        /// @return @p idx 'th element in the ring buffer
        LogEntry get(unsigned idx) const;

        /// Format and log a printf-style message
        void format(const char *fmt, ...);

        /// Format and log a printf-style message with file name and line
        /// number of code printing this message included
        void format(const char *file, unsigned line,
                    const char *fmt, ...);

        /// @return Log description
        const char *description() const { return descr; }

        /// @return Log default severity
        LogLevel defaultSeverity() const { return defaultLevel; }

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

#define PROVIDER_LOG_ERR(_fmt, _args...) \
    solarflare::Logger::errorLog.format(__FILE_REL__, __LINE__, \
                                        _fmt, ##_args)

#define PROVIDER_LOG_EVT(_fmt, _args...) \
    solarflare::Logger::eventLog.format(__FILE_REL__, __LINE__, \
                                        _fmt, ##_args)

#define PROVIDER_LOG_DBG(_fmt, _args...) \
    solarflare::Logger::debugLog.format(__FILE_REL__, __LINE__, \
                                        _fmt, ##_args)

#endif   // SOLARFLARE_SF_CORE_H
