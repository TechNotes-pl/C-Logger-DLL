#include "pch.h" 
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
//#include "ConsoleLogger.h"
//#include "FileLogger.h"

#if defined(_WIN32)
 #include <winsock2.h>
#else
 #include <pthread.h>
 #include <sys/time.h>
 #include <sys/syscall.h>
 #include <unistd.h>
#endif
#include "C_logger.h"

#if defined(_WIN32)
int gettimeofday(struct timeval* tv, void* tz)
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
#endif

long getCurrentThreadID(void)
{
#if defined(_WIN32)
    return GetCurrentThreadId();
#elif __linux__
    return syscall(SYS_gettid);
#elif defined(__APPLE__) && defined(__MACH__)
    return syscall(SYS_thread_selfid);
#else
    return (long) pthread_self();
#endif
}

#ifdef __linux__
#define fopen_s(pFile,filename,mode) ((*(pFile))=fopen((filename),  (mode)))==NULL
#endif



#include <iostream>
#include <fstream>

using namespace std;

// Returns -1 on permission error
std::ifstream::pos_type getFileSize_c(const char* filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}

// See: https://cplusplus.com/doc/tutorial/files/
long getFileSize_my(const char* filename)
{
    std::ifstream::pos_type size{};

    ifstream file(filename, ios::in | ios::binary | ios::ate);

    if (file.is_open())
    {
        size = file.tellg();
        file.close();
    }

    return size;
}


long getFileSize_org(const char* filename)
{
    FILE* fp;
    long size;

    if ((fp = fopen(filename, "rb")) == NULL) {
        return 0;
    }
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fclose(fp);
    return size;
}

long getFileSize(const char* fileName)
{
    FILE* filepoint = nullptr;
    errno_t err = 0;
    long size = 0;
    char buffer[ErrBufferLen + 1]{};

    if ((err = fopen_s(&filepoint, fileName, "rb")) != 0) {
        // File could not be opened. filepoint was set to NULL
        // error code is returned in err.
        // error message can be retrieved with strerror(err);
        strerror_s(buffer, ErrBufferLen, err);
        fprintf(stderr, "ERROR: getFileSize() cannot open file '%s': %s\n", fileName, buffer);
        // If your environment insists on using so called secure functions, use this instead:
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


char getLevelChar(LogLevel level)
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

void getTimestamp(const struct timeval* time, char* timestamp, size_t size)
{
    //char buffer[ErrBufferLen + 1]{};
    struct tm calendar {};
    time_t sec = time->tv_sec; /* a necessary variable to avoid a runtime error on Windows */
    
    assert(size >= 25);

    // Convert given time since epoch (a time_t value pointed to by timer) into calendar time
    localtime_r(&sec, &calendar);
    // Create timestamp string
    strftime(timestamp, size, "%y-%m-%d %H:%M:%S", &calendar);
    // Add microsecons
    sprintf(&timestamp[17], ".%06ld", (long) time->tv_usec);
}

void getBackupFileName(const char* basename, unsigned char index, char* backupname, size_t size)
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

