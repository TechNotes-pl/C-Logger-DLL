// Logger.cpp : Defines the exported functions for the DLL.
#include "pch.h" 
#include "ConsoleLogger.h"
#include "FileLogger.h"

bool console_log = false;
bool file_log = false;

ConsoleLogger clog;
FileLogger flog;

int InitConsoleLogger(FILE* output) 
{
	console_log = true;
	return clog.logger_initConsoleLogger(output);
}

int InitFileLogger(const char* filename, long maxFileSize, int maxBackupFiles)
{
	file_log = true;
	return flog.logger_initFileLogger(filename, maxFileSize, maxBackupFiles);
}

void SetLevel(LogLevel level) 
{
	if(console_log)
		clog.logger_setLevel(level);
	if (file_log)
		flog.logger_setLevel(level);
}

void LoggerLog(LogLevel level, const char* file, int line, const char* fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	if(console_log)
		clog.logger_log(level, file, line, fmt, args);
	if (file_log)
		flog.logger_log(level, file, line, fmt, args);
	va_end(args);
}
