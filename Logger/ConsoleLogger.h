#pragma once
#include "LoggerBase.h"

class ConsoleLogger :
    public LoggerBase
{
public:

    static int logger_initConsoleLogger(FILE* output_stream);

    void logger_log(LogLevel level, const char* file, int line, const char* fmt, va_list arg) override;
};

