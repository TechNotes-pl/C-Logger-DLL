/// @Logger.h - Contains declarations of C logger functions
#pragma once

#include <stdio.h>
#include <string.h>

#ifdef LOGGER_LIBRARY_EXPORTS
#define LOGGER_LIBRARY_API __declspec(dllexport)
#else
#define LOGGER_LIBRARY_API __declspec(dllimport)
#endif

/**
 * @brief Logger level enum
*/
extern "C" LOGGER_LIBRARY_API typedef enum {
    LogLevel_TRACE = 1,
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

/**
* \brief Initialize the logger as a console logger.
* ex. usage: InitConsoleLogger(stderr);
* If the file pointer is NULL, stdout will be used.
* \param[in] output Outpur stream, use stdout or stderr.
* @return Non-zero int value upon success or 0 on error
* \retval        ERR_SUCCESS    The function is successfully executed
* \retval        ERR_FAILURE    An error occurred
*/
extern "C" LOGGER_LIBRARY_API int InitConsoleLogger(FILE * output);

/**
  * \brief Initialize logging to file - opens a log file and gets the current size . 
  *  
  * \param[in] filename The name of the output log file
  * \param[in] maxFileSize The maximum number of bytes to write to any one file
  * \param[in] maxBackupFiles The maximum number of files for backup
  * \return Non-zero value upon success or 0 on error
  */
extern "C" LOGGER_LIBRARY_API int InitFileLogger(const char* filename, long maxFileSize = 1024 * 1024, unsigned char maxBackupFiles = 5);

/**
  * \brief Set the log level.
  * Message levels lower than selected value will be discarded.
  * The default log level is INFO.
  * \param[in] level Log level enum
  */
extern "C" LOGGER_LIBRARY_API void SetLevel(LogLevel level);


/**
 * @brief Put a message into log.
 * Make sure to call one of the following initialize functions before starting logging:
 *      InitConsoleLogger()
 *      InitFileLogger()
 * @param[in] level Log level enum
 * @param[in] file Current code file name
 * @param[in] line Line number in current code file
 * @param[in] fmt Line format
 * @param[in] ... Format arguments
 * @return 
*/
extern "C" LOGGER_LIBRARY_API void LoggerLog(LogLevel level, const char* file, int line, const char* fmt, ...);
