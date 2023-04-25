// Logger.cpp : Defines the exported functions for the DLL.
#include "pch.h" 
#include <stdarg.h>
#include "Logger.h"
#include "C_logger.h"

int InitConsoleLogger(FILE* output) {
	return logger_initConsoleLogger(output);
}

void SetLevel(LogLevel level) {
	logger_setLevel(level);
}

int InitFileLogger(const char* filename, long maxFileSize, unsigned char maxBackupFiles)
{
	return logger_initFileLogger(filename, maxFileSize, maxBackupFiles);
}

void LoggerLog(LogLevel level, const char* file, int line, const char* fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	logger_log(level, file, line, fmt, args);
	va_end(args);
}
