#ifndef LOGGER_H
#define LOGGER_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdio.h>
#include <string.h>
#include "Logger.h"

constexpr int ErrBufferLen = 80;
const auto kMaxFileNameLen = 255; /* without null character */
const auto kDefaultMaxFileSize = 1048576L; /* 1 MB */

void lock(void);
void unlock(void);

#if defined(_WIN32)
int gettimeofday(struct timeval* tv, void* tz);
struct tm* localtime_r(const time_t* timep, struct tm* result);
#endif
long getCurrentThreadID(void);
//std::ifstream::pos_type getFileSize(const char* filename);
long getFileSize(const char* filename);
char getLevelChar(LogLevel level);
void getBackupFileName(const char* basename, unsigned char index, char* backupname, size_t size);
void getTimestamp(const struct timeval* time, char* timestamp, size_t size);

/**
 * \brief Initialize the logger as a console logger.
 * If the file pointer is NULL, stdout will be used.
 *
 * @param[in] output A file pointer. Make sure to set stdout or stderr.
 * @return Non-zero value upon success or 0 on error
 */
//int logger_initConsoleLogger(FILE* output);

/**
 * \brief Initialize the logger as a file logger.
 * If the filename is NULL, return without doing anything.
 *
 * @param[in] filename The name of the output file
 * @param[in] maxFileSize The maximum number of bytes to write to any one file
 * @param[in] maxBackupFiles The maximum number of files for backup
 * @return Non-zero value upon success or 0 on error
 */
//int logger_initFileLogger(const char* filename, long maxFileSize, unsigned char maxBackupFiles);

/**
 * \brief Set the log level.
 * Message levels lower than this value will be discarded.
 * The default log level is INFO.
 *
 * @param[in] level A log level
 */
//void logger_setLevel(LogLevel level);

/**
 * \brief Get the log level that has been set.
 * The default log level is INFO.
 *
 * @return The log level
 */
//LogLevel logger_getLevel(void);

/**
 * \brief Check if a message of the level would actually be logged.
 *
 * @return Non-zero value if the log level is enabled
 */
//int logger_isEnabled(LogLevel level);

/**
 * \brief Flush automatically.
 * Auto flush is off in default.
 *
 * @param[in] interval A fulsh interval in milliseconds. Switch off if 0 or a negative integer.
 */
//void logger_autoFlush(long interval);

/**
 * \brief Log a message.
 * Make sure to call one of the following initialize functions before starting logging.
 * - logger_initConsoleLogger()
 * - logger_initFileLogger()
 *
 * @param[in] level A log level
 * @param[in] file A file name string
 * @param[in] line A line number
 * @param[in] fmt A format string
 * @param[in] ... Additional arguments
 */
//void logger_log(LogLevel level, const char* file, int line, const char* fmt, va_list arg);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* LOGGER_H */
