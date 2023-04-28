#pragma once
#include "C_logger.h"
#include "Locker.h"

/* Console logger */
static class LoggerBase : public Locker
{
public:
    inline static FILE* output;
    inline static unsigned long long flushedTime;

    LoggerBase()
    {
        m_logLevel = LogLevel_INFO;
    }

    static void logger_setLevel(LogLevel level) { m_logLevel = level; }
    static LogLevel logger_getLevel(void) { return m_logLevel; }
    static int logger_isEnabled(LogLevel level) { return m_logLevel <= level; }

    /**
      * \brief Flush buffered log messages.
      */
    void logger_flush() {
        if (isInitialized()) {
            fflush(output);
        }
    }

    static void logger_autoFlush(long interval) { m_flushInterval = interval > 0 ? interval : 0; }

    virtual void logger_log(LogLevel level, const char* file, int line, const char* fmt, va_list arg) = 0;

    static long vflog(FILE* fp, char levelc, const char* timestamp, long threadID,
        const char* file, int line, const char* fmt, va_list arg,
        unsigned long long currentTime, unsigned long long* flushedTime);
    
private:
    inline static LogLevel m_logLevel;
    inline static long m_flushInterval; /* msec, 0 is auto flush off */
};