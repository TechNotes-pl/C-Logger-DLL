#include "pch.h" 
#include "LoggerBase.h"



long LoggerBase::vflog(FILE* fp, char levelc, const char* timestamp, long threadID,
    const char* file, int line, const char* fmt, va_list arg,
    unsigned long long currentTime, unsigned long long* flushedTime)
{
    int size;
    long totalsize = 0;

    if ((size = fprintf(fp, "%c %s %ld %s:%d: ", levelc, timestamp, threadID, file, line)) > 0) {
        totalsize += size;
    }
    if ((size = vfprintf(fp, fmt, arg)) > 0) {
        totalsize += size;
    }
    if ((size = fprintf(fp, "\n")) > 0) {
        totalsize += size;
    }
    if (m_flushInterval > 0) {
        if (currentTime - *flushedTime > m_flushInterval) {
            fflush(fp);
            *flushedTime = currentTime;
        }
    }
    return totalsize;
}