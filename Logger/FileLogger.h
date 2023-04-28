#pragma once
#include "LoggerBase.h"

class FileLogger :
    public LoggerBase
{
public:
    inline static char filename[kMaxFileNameLen + 1];
    inline static long maxFileSize;
    inline static unsigned char maxBackupFiles;
    inline static long currentFileSize;

    static int logger_initFileLogger(const char* filename, long maxFileSize, unsigned char maxBackupFiles);

    void logger_log(LogLevel level, const char* file, int line, const char* fmt, va_list arg) override;

    static int rotateLogFiles();
};

