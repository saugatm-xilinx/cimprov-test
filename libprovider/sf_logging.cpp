/***************************************************************************//*! \file liprovider/sf_logging.cpp
** <L5_PRIVATE L5_SOURCE>
** \author  OktetLabs
**  \brief  CIM Provider
**   \date  2013/10/02
**    \cop  (c) Solarflare Communications Inc.
** </L5_PRIVATE>
*//*
\**************************************************************************/

#include "sf_logging.h"

namespace solarflare
{
    using cimple::Auto_Mutex;
    
    void LogEntry::printLog() const
    {
        static const cimple::Log_Level mapping[] = 
        {
            cimple::LL_FATAL,
            cimple::LL_ERR,
            cimple::LL_WARN,
            cimple::LL_INFO,
            cimple::LL_DBG,
        };
        if (errorCode == 0)
            cimple::log(mapping[level],
                        file.c_str(), line, "%s", messageStr.c_str());
        else
            cimple::log(mapping[level],
                        file.c_str(), line, "[errorCode = %u] %s",
                        errorCode, messageStr.c_str());
    }

    String Logger::doFormat(const char *fmt, va_list args)
    {
        Auto_Mutex excl(lock);
        
        formatter.clear();
        formatter.vformat(fmt, args);
        return formatter.data();
    }

    void Logger::put(const LogEntry& e)
    {
        if (enabled)
        {
            Auto_Mutex excl(lock);
            entries[serial % size] = e;
            entries[serial % size].id(serial);
            serial++;
            e.printLog();
        }
    }

    void Logger::format(const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        log(doFormat(fmt, args));
        va_end(args);
    }
    
    void Logger::format(const char *file, unsigned line,
                        const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        log(file, line, doFormat(fmt, args));
        va_end(args);
    }

    void Logger::clear(void)
    {
        Auto_Mutex excl(lock);
        serial = 0;
    }

    unsigned Logger::currentSize() const
    {
        Auto_Mutex excl(lock);
        return serial > size ? size : serial;
    }

    LogEntry Logger::get(unsigned idx) const
    {
        Auto_Mutex excl(lock);
        LogEntry le;
        if (serial > 0)
        {
            unsigned from = serial >= size ? (serial - size) % size : 0;

            le = entries[(from + idx) % size];
        }
        return le;
    }

    Logger &Logger::getErrorLog()
    {
        static Logger errorLog(LogError, 128, "Error log");

        return errorLog;
    }

    Logger &Logger::getEventLog()
    {
        static Logger eventLog(LogInfo, 128, "Event log");

        return eventLog;
    }
 
    // This log is disabled by default
    Logger &Logger::getDebugLog()
    {
        static Logger debugLog(LogDebug, 1024,
                               "Debugging log", false);

        return debugLog;
    }
 
    Logger *const Logger::getKnownLogs()
    {
        static Logger *const knownLogs[] = {
            &Logger::getErrorLog(),
            &Logger::getEventLog(),
            &Logger::getDebugLog(),
            NULL
        };

        return knownLogs;
    }
}
