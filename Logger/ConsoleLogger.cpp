#include "pch.h"
#include <assert.h>
#if defined(_WIN32)
#include <winsock.h> // for struct timeval in Windows
#endif
#include "ConsoleLogger.h"

int ConsoleLogger::logger_initConsoleLogger(FILE* output_stream)
{
    output_stream = (output_stream != NULL) ? output_stream : stdout;
    if (output_stream != stdout && output_stream != stderr) {
        assert(0 && "output must be stdout or stderr");
        return 0;
    }

    init();
    lock();
    output = output_stream;
    unlock();
    return 1;
}

void ConsoleLogger::logger_log(LogLevel level, const char* file, int line, const char* fmt, va_list arg) 
{
    struct timeval now;
    unsigned long long currentTime{}; /* milliseconds */
    char levelc{};
    const auto tmsBuffLen = 32;
    char timestamp[tmsBuffLen]{};
    const long threadID = getCurrentThreadID();

    if (isInitialized && logger_isEnabled(level))
    {
        gettimeofday(&now, NULL);
        currentTime = static_cast<unsigned long long>(now.tv_sec) * 1000 + now.tv_usec / 1000;
        levelc = getLevelChar(level);
        getTimestamp(&now, timestamp, tmsBuffLen);

        lock();

        vflog(output, levelc, timestamp, threadID, file, line, fmt, arg, currentTime, &flushedTime);

        unlock();
    }
}
