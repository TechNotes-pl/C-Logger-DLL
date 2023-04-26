#include "pch.h" 
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#if defined(_WIN32) || defined(_WIN64)
 #include <winsock2.h>
#else
 #include <pthread.h>
 #include <sys/time.h>
 #include <sys/syscall.h>
 #include <unistd.h>
#endif /* defined(_WIN32) || defined(_WIN64) */
#include "C_logger.h"

 /* Logger type */
enum {
   
    kConsoleLogger = 1 << 0,
    kFileLogger = 1 << 1   
};

const auto kMaxFileNameLen = 255; /* without null character */
const auto kDefaultMaxFileSize = 1048576L; /* 1 MB */

/* Console logger */
static struct {
    FILE* output;
    unsigned long long flushedTime;
} s_clog;

/* File logger */
static struct {
    FILE* output;
    char filename[kMaxFileNameLen + 1];
    long maxFileSize;
    unsigned char maxBackupFiles;
    long currentFileSize;
    unsigned long long flushedTime;
} s_flog;

// A volatile type qualifier means that an object can be modified in the program by the hardware.
static volatile int s_logger;
static volatile LogLevel s_logLevel = LogLevel_INFO;
static volatile long s_flushInterval = 0; /* msec, 0 is auto flush off */
static volatile int s_initialized = 0; /* false */

#if defined(_WIN32) || defined(_WIN64)
static CRITICAL_SECTION s_mutex;
#else
static pthread_mutex_t s_mutex;
#endif /* defined(_WIN32) || defined(_WIN64) */

static void init(void)
{
    if (s_initialized) {
        return;
    }
#if defined(_WIN32) || defined(_WIN64)
    InitializeCriticalSection(&s_mutex);
#else
    pthread_mutex_init(&s_mutex, NULL);
#endif /* defined(_WIN32) || defined(_WIN64) */
    s_initialized = 1; /* true */
}

static void lock(void)
{
#if defined(_WIN32) || defined(_WIN64)
    EnterCriticalSection(&s_mutex);
#else
    pthread_mutex_lock(&s_mutex);
#endif /* defined(_WIN32) || defined(_WIN64) */
}

static void unlock(void)
{
#if defined(_WIN32) || defined(_WIN64)
    LeaveCriticalSection(&s_mutex);
#else
    pthread_mutex_unlock(&s_mutex);
#endif /* defined(_WIN32) || defined(_WIN64) */
}

#if defined(_WIN32) || defined(_WIN64)
static int gettimeofday(struct timeval* tv, void* tz)
{
    const UINT64 epochFileTime = 116444736000000000ULL;
    FILETIME ft;
    ULARGE_INTEGER li{};
    UINT64 t;

    if (tv == NULL) {
        return -1;
    }
    GetSystemTimeAsFileTime(&ft);
    li.LowPart = ft.dwLowDateTime;
    li.HighPart = ft.dwHighDateTime;
    t = (li.QuadPart - epochFileTime) / 10;
    tv->tv_sec = (long) (t / 1000000);
    tv->tv_usec = t % 1000000;
    return 0;
}

static struct tm* localtime_r(const time_t* timep, struct tm* result)
{
    localtime_s(result, timep);
    return result;
}
#endif /* defined(_WIN32) || defined(_WIN64) */

static long getCurrentThreadID(void)
{
#if defined(_WIN32) || defined(_WIN64)
    return GetCurrentThreadId();
#elif __linux__
    return syscall(SYS_gettid);
#elif defined(__APPLE__) && defined(__MACH__)
    return syscall(SYS_thread_selfid);
#else
    return (long) pthread_self();
#endif /* defined(_WIN32) || defined(_WIN64) */
}

int logger_initConsoleLogger(FILE* output)
{
    output = (output != NULL) ? output : stdout;
    if (output != stdout && output != stderr) {
        assert(0 && "output must be stdout or stderr");
        return 0;
    }

    init();
    lock();
    s_clog.output = output;
    s_logger |= kConsoleLogger;
    unlock();
    return 1;
}



static long getFileSize(const char* fileName)
{
    FILE* filepoint = nullptr;
    errno_t err = 0;
    long size = 0;
    char buffer[ErrBufferLen + 1]{};

    if ((err = fopen_s(&filepoint, fileName, "r")) != 0) {
        // File could not be opened. filepoint was set to NULL
        // error code is returned in err.
        // error message can be retrieved with strerror(err);
        strerror_s(buffer, ErrBufferLen, err);
        fprintf(stderr, "cannot open file '%s': %s\n", fileName, buffer);
        // If your environment insists on using so called secure
        // functions, use this instead:
        //      char buf[strerrorlen_s(err) + 1];
        //      strerror_s(buf, sizeof buf, err);
        //      fprintf_s(stderr, "cannot open file '%s': %s\n", fileName, buf);
    }
    else 
    {
        // File was opened, filepoint can be used to read the stream.
        fseek(filepoint, 0, SEEK_END);
        size = ftell(filepoint);
        fclose(filepoint);
    }

    return size;
}

int logger_initFileLogger(const char* filename, long maxFileSize, unsigned char maxBackupFiles)
{
    if (filename == NULL) {
        assert(0 && "filename must not be NULL");
        return 0;
    }
    if (strlen(filename) > kMaxFileNameLen) {
        assert(0 && "filename exceeds the maximum number of characters");
        return 0;
    }

    init();
    lock();
    if (s_flog.output != NULL) { /* reinit */
        fclose(s_flog.output);
    }

    errno_t err;
    if ((err = fopen_s(&s_flog.output, filename, "a")) != 0) {
        char buffer[ErrBufferLen + 1]{};
        strerror_s(buffer, ErrBufferLen, err);
        fprintf(stderr, "ERROR: logger: Failed to open file '%s': %s\n", filename, buffer);
        unlock();
        return 0;
    }

    s_flog.currentFileSize = getFileSize(filename);
    strncpy(s_flog.filename, filename, sizeof(s_flog.filename));
    s_flog.maxFileSize = (maxFileSize > 0) ? maxFileSize : kDefaultMaxFileSize;
    s_flog.maxBackupFiles = maxBackupFiles;
    s_logger |= kFileLogger;

    unlock();
    return 1;
}

void logger_setLevel(LogLevel level)
{
    s_logLevel = level;
}

LogLevel logger_getLevel(void)
{
    return s_logLevel;
}

int logger_isEnabled(LogLevel level)
{
    return s_logLevel <= level;
}

void logger_autoFlush(long interval)
{
    s_flushInterval = interval > 0 ? interval : 0;
}

static int hasFlag(int flags, int flag)
{
    return (flags & flag) == flag;
}

void logger_flush()
{
    if (s_logger == 0 || !s_initialized) {
        assert(0 && "logger is not initialized");
        return;
    }

    if (hasFlag(s_logger, kConsoleLogger)) {
        fflush(s_clog.output);
    }
    if (hasFlag(s_logger, kFileLogger)) {
        fflush(s_flog.output);
    }
}

static char getLevelChar(LogLevel level)
{
    switch (level) {
        case LogLevel_TRACE: return 'T';
        case LogLevel_DEBUG: return 'D';
        case LogLevel_INFO:  return 'I';
        case LogLevel_WARN:  return 'W';
        case LogLevel_ERROR: return 'E';
        case LogLevel_FATAL: return 'F';
        default: return ' ';
    }
}

static void getTimestamp(const struct timeval* time, char* timestamp, size_t size)
{
    //char buffer[ErrBufferLen + 1]{};
    struct tm calendar {};
    time_t sec = time->tv_sec; /* a necessary variable to avoid a runtime error on Windows */
    
    auto size1 = strlen(timestamp);
    assert(size >= 25);

    // Convert given time since epoch (a time_t value pointed to by timer) into calendar time
    localtime_r(&sec, &calendar);
    // Create timestamp string
    strftime(timestamp, size, "%y-%m-%d %H:%M:%S", &calendar);
    // Add microsecons
    sprintf(&timestamp[17], ".%06ld", (long) time->tv_usec);
}

static void getBackupFileName(const char* basename, unsigned char index, char* backupname, size_t size)
{
    char indexname[5];

    assert(size >= strlen(basename) + sizeof(indexname));

    strncpy(backupname, basename, size);
    if (index > 0) {
        //sprintf(indexname, ".%d", index);
        //strncat(backupname, indexname, strlen(indexname));
        sprintf_s(indexname, ".%d", index);
        strncat(backupname, indexname, strlen(indexname));
    }
}

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

static int rotateLogFiles()
{
    int i;
    /* backup filename: <filename>.xxx (xxx: 1-255) */
    char src[kMaxFileNameLen + 5], dst[kMaxFileNameLen + 5]; /* with null character */

    if (s_flog.currentFileSize < s_flog.maxFileSize) {
        return s_flog.output != NULL;
    }
    fclose(s_flog.output);
    for (i = (int) s_flog.maxBackupFiles; i > 0; i--) {
        getBackupFileName(s_flog.filename, i - 1, src, sizeof(src));
        getBackupFileName(s_flog.filename, i, dst, sizeof(dst));
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


    errno_t err;

    if ((err = fopen_s(&s_flog.output, s_flog.filename, "a")) != 0) {
        char buffer[ErrBufferLen + 1]{};
        strerror_s(buffer, ErrBufferLen, err);
        fprintf(stderr, "ERROR: logger: Failed to open file '%s': %s\n", s_flog.filename, buffer);
        return 0;
    }
    s_flog.currentFileSize = getFileSize(s_flog.filename);
    return 1;
}

static long vflog(FILE* fp, char levelc, const char* timestamp, long threadID,
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
    if (s_flushInterval > 0) {
        if (currentTime - *flushedTime > s_flushInterval) {
            fflush(fp);
            *flushedTime = currentTime;
        }
    }
    return totalsize;
}

void logger_log(LogLevel level, const char* file, int line, const char* fmt, va_list arg)
{
    struct timeval now {};
    unsigned long long currentTime{}; /* milliseconds */
    char levelc{};
    const auto tmsBuffLen = 32;
    char timestamp[tmsBuffLen]{};
    const long threadID = getCurrentThreadID();

    if (s_logger == 0 || !s_initialized) {
        assert(0 && "logger is not initialized");
        return;
    }

    if (!logger_isEnabled(level)) {
        return;
    }

    gettimeofday(&now, NULL);
    currentTime = static_cast<unsigned long long>(now.tv_sec) * 1000 + now.tv_usec / 1000;
    levelc = getLevelChar(level);
    getTimestamp(&now, timestamp, tmsBuffLen);

    lock();

    if (hasFlag(s_logger, kConsoleLogger)) {
        vflog(s_clog.output, levelc, timestamp, threadID,
                file, line, fmt, arg, currentTime, &s_clog.flushedTime);
    }

    if (hasFlag(s_logger, kFileLogger)) {
        if (rotateLogFiles()) {
            s_flog.currentFileSize += vflog(s_flog.output, levelc, timestamp, threadID,
                    file, line, fmt, arg, currentTime, &s_flog.flushedTime);
        }
    }

    unlock();
}

