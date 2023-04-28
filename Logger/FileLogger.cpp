#include "pch.h"
#include <assert.h>
#if defined(_WIN32)
#include <winsock.h> // for struct timeval in Windows
#endif
#include "FileLogger.h"
//#include "C_logger.h"

static int isFileExist(const char* fileName)
{
    FILE* filepoint = nullptr;
    errno_t err = 0;

    if ((err = fopen_s(&filepoint, fileName, "r")) != 0)
    {
        return 0;
    }
    else
    {
        fclose(filepoint);
        return 1;
    }
}


int FileLogger::logger_initFileLogger(const char* fileName, long maxFileSize, int maxBackupFiles)
{
    if (fileName == NULL) {
        assert(0 && "filename must not be NULL");
        return 0;
    }
    if (strlen(fileName) > kMaxFileNameLen) {
        assert(0 && "filename exceeds the maximum number of characters");
        return 0;
    }

    init();
    lock();

    if (output != NULL) { /* reinit */
        fclose(output);
    }

    // Get current file size when file is not opened yet
    currentFileSize = getFileSize(fileName);

    errno_t err;
    if ((err = fopen_s(&output, fileName, "a")) != 0) {
        char buffer[ErrBufferLen + 1]{};
        strerror_s(buffer, ErrBufferLen, err);
        fprintf(stderr, "ERROR: logger: Failed to open file '%s': %s\n", fileName, buffer);
        unlock();
        return 0;
    }

    strncpy_s(filename, fileName, sizeof(filename));
    maxFileSize = (maxFileSize > 0) ? maxFileSize : kDefaultMaxFileSize;
    maxBackupFiles = maxBackupFiles;

    unlock();

    return 1;
}


void FileLogger::logger_log(LogLevel level, const char* file, int line, const char* fmt, va_list arg)
{
    struct timeval now{};
    unsigned long long currentTime{}; /* milliseconds */
    char levelc{};
    const auto tmsBuffLen = 32;
    char timestamp[tmsBuffLen]{};
    const long threadID = getCurrentThreadID();

    if (isInitialized() && logger_isEnabled(level))
    {
        gettimeofday(&now, NULL);
        currentTime = static_cast<unsigned long long>(now.tv_sec) * 1000 + now.tv_usec / 1000;
        levelc = getLevelChar(level);
        getTimestamp(&now, timestamp, tmsBuffLen);

        lock();

        if (rotateLogFiles()) {
            currentFileSize += vflog(output, levelc, timestamp, threadID,
                file, line, fmt, arg, currentTime, &flushedTime);
        }

        unlock();
    }
}


int FileLogger::rotateLogFiles()
{
    /* backup filename: <filename>.xxx (xxx: 1-255) */
    char src[kMaxFileNameLen + 5]{};
    char dst[kMaxFileNameLen + 5]{}; /* with null character */

    if (currentFileSize < maxFileSize) {
        return output != NULL;
    }

    fclose(output);

    for (int i = maxBackupFiles; i > 0; i--) {
        getBackupFileName(filename, i - 1, src, sizeof(src));
        getBackupFileName(filename, i, dst, sizeof(dst));
        if (isFileExist(dst)) {
            if (remove(dst) != 0) {
                fprintf(stderr, "ERROR: logger: Failed to remove file: `%s`\n", dst);
            }
        }
        if (isFileExist(src)) {
            if (rename(src, dst) != 0) {
                fprintf(stderr, "ERROR: logger: Failed to rename file: `%s` -> `%s`\n", src, dst);
            }
        }
    }

    currentFileSize = getFileSize(filename);

    errno_t err;
    if ((err = fopen_s(&output, filename, "a")) != 0) {
        char buffer[ErrBufferLen + 1]{};
        strerror_s(buffer, ErrBufferLen, err);
        fprintf(stderr, "ERROR: logger: Failed to open file '%s': %s\n", filename, buffer);
        return 0;
    }

    return 1;
}