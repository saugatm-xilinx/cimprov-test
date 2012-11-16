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
        cimple::log(mapping[level],
                    "", errorCode, "%s", messageStr.c_str());
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

    
    Logger Logger::errorLog(LogError, 128, "Error log");
    Logger Logger::eventLog(LogInfo, 128, "Event log");
    Logger Logger::debugLog(LogDebug, 1024, "Debugging log");

    Logger *const Logger::knownLogs[] = {
        &Logger::errorLog,
        &Logger::eventLog,
        &Logger::debugLog,
        NULL
    };
}
