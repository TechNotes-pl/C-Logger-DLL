#pragma once
#include "LoggerBase.h"

class FileLogger :
    public LoggerBase
{
public:

    int logger_initFileLogger(const char* filename, long maxFileSize, int maxBackupFiles);

    void logger_log(LogLevel level, const char* file, int line, const char* fmt, va_list arg) override;

private:
    int rotateLogFiles();

private:
    long currentFileSize{};
    char filename[kMaxFileNameLen + 1]{};
    long maxFileSize{};
    int maxBackupFiles{};
    
};

