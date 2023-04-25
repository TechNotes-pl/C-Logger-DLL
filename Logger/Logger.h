// Logger.h - Contains declarations of C logger functions
#pragma once

#include <stdio.h>
#include <string.h>

#ifdef LOGGER_LIBRARY_EXPORTS
#define LOGGER_LIBRARY_API __declspec(dllexport)
#else
#define LOGGER_LIBRARY_API __declspec(dllimport)
#endif

extern "C" LOGGER_LIBRARY_API typedef enum {
    LogLevel_TRACE,
    LogLevel_DEBUG,
    LogLevel_INFO,
    LogLevel_WARN,
    LogLevel_ERROR,
    LogLevel_FATAL,
} LogLevel;

#if defined(_WIN32)
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#define LOG_TRACE(fmt, ...) LoggerLog(LogLevel_TRACE, __FILENAME__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) LoggerLog(LogLevel_DEBUG, __FILENAME__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)  LoggerLog(LogLevel_INFO , __FILENAME__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)  LoggerLog(LogLevel_WARN , __FILENAME__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) LoggerLog(LogLevel_ERROR, __FILENAME__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_FATAL(fmt, ...) LoggerLog(LogLevel_FATAL, __FILENAME__, __LINE__, fmt, ##__VA_ARGS__)

extern "C" LOGGER_LIBRARY_API int InitConsoleLogger(FILE * output);

extern "C" LOGGER_LIBRARY_API int InitFileLogger(const char* filename, long maxFileSize, unsigned char maxBackupFiles);

extern "C" LOGGER_LIBRARY_API void SetLevel(LogLevel level);

extern "C" LOGGER_LIBRARY_API void LoggerLog(LogLevel level, const char* file, int line, const char* fmt, ...);
