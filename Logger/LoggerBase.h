#pragma once
#include "C_logger.h"
#include "Locker.h"

/* Console logger */
class LoggerBase : public Locker
{
public:
    FILE* output{};
    unsigned long long flushedTime{};

    LoggerBase()
    {
        m_logLevel = LogLevel_INFO;
    }

    void logger_setLevel(LogLevel level) { m_logLevel = level; }
    LogLevel logger_getLevel(void) { return m_logLevel; }
    int logger_isEnabled(LogLevel level) { return m_logLevel <= level; }

    /**
      * \brief Flush buffered log messages.
      */
    void logger_flush() {
        if (isInitialized()) {
            fflush(output);
        }
    }

    void logger_autoFlush(long interval) { m_flushInterval = interval > 0 ? interval : 0; }

    virtual void logger_log(LogLevel level, const char* file, int line, const char* fmt, va_list arg) = 0;

    long vflog(FILE* fp, char levelc, const char* timestamp, long threadID,
        const char* file, int line, const char* fmt, va_list arg,
        unsigned long long currentTime, unsigned long long* flushedTime);
    
private:
    LogLevel m_logLevel{};
    long m_flushInterval{}; /* msec, 0 is auto flush off */
};