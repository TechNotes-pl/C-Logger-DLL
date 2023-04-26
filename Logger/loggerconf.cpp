#include "pch.h" 

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "C_logger.h"
#include "loggerconf.h"

using namespace CLog;

 int LoggerConf::logger_configure(const char* filename) {
     FILE* fp;
     char line[kMaxLineLen];
     errno_t err;

     if (filename == NULL) {
         assert(0 && "filename must not be NULL");
         return 0;
     }

     reset();

     if ((err = fopen_s(&fp, filename, "r")) != 0) {
         char buffer[ErrBufferLen + 1]{};
         strerror_s(buffer, ErrBufferLen, err);
         fprintf(stderr, "ERROR: loggerconf: Failed to open file '%s': %s\n", m_filename, buffer);
         return 0;
     }

     while (fgets(line, sizeof(line), fp) != NULL) {
         removeComments(line);
         trim(line);
         if (line[0] == '\0') {
             continue;
         }
         parseLine(line);
     }
     fclose(fp);

     if (hasFlag(s_logger, kConsoleLogger)) {
         if (!logger_initConsoleLogger(m_filepoint)) {
             return 0;
         }
     }
     if (hasFlag(s_logger, kFileLogger)) {
         if (!logger_initFileLogger(m_filename, maxFileSize, maxBackupFiles)) {
             return 0;
         }
     }
     if (s_logger == 0) {
         return 0;
     }
     return 1;
 }

 void LoggerConf::reset(void) {
     s_logger = 0;
     m_filepoint = nullptr;   // ??

     memset(m_filename, 0, kMaxFileNameLen);
     maxFileSize = 0;
     maxBackupFiles = 0;
 }

 void LoggerConf::removeComments(char* s) {
     int i;

     for (i = 0; s[i] != '\0'; i++) {
         if (s[i] == '#') {
             s[i] = '\0';
         }
     }
 }

void LoggerConf::trim(char* s) {
    size_t len;
    int i;

    if (s == NULL) {
        return;
    }
    len = strlen(s);
    if (len == 0) {
        return;
    }
    /* trim right */
    for (i = len - 1; i >= 0 && isspace(s[i]); i--) {}
    s[i + 1] = '\0';
    /* trim left */
    for (i = 0; s[i] != '\0' && isspace(s[i]); i++) {}
    memmove(s, &s[i], len - i);
    s[len - i] = '\0';
}

void LoggerConf::parseLine(char* line) {
    auto key = strtok(line, "=");
    auto val = strtok(NULL, "=");

    if (strcmp(key, "level") == 0) {
        logger_setLevel(parseLevel(val));
    }
    else if (strcmp(key, "autoFlush") == 0) {
        logger_autoFlush(atol(val));
    }
    else if (strcmp(key, "logger") == 0) {
        if (strcmp(val, "console") == 0) {
            s_logger |= kConsoleLogger;
        }
        else if (strcmp(val, "file") == 0) {
            s_logger |= kFileLogger;
        }
        else {
            fprintf(stderr, "ERROR: loggerconf: Invalid logger: `%s`\n", val);
            s_logger = 0;
        }
    }
    else if (strcmp(key, "logger.console.output") == 0) {
        if (strcmp(val, "stdout") == 0) {
            m_filepoint = stdout;
        }
        else if (strcmp(val, "stderr") == 0) {
            m_filepoint = stderr;
        }
        else {
            fprintf(stderr, "ERROR: loggerconf: Invalid logger.console.output: `%s`\n", val);
            m_filepoint = NULL;
        }
    }
    else if (strcmp(key, "logger.file.filename") == 0) {
        strncpy(m_filename, val, sizeof(m_filename));
    }
    else if (strcmp(key, "logger.file.maxFileSize") == 0) {
        maxFileSize = atol(val);
    }
    else if (strcmp(key, "logger.file.maxBackupFiles") == 0) {
        auto nfiles = atoi(val);
        if (nfiles < 0) {
            fprintf(stderr, "ERROR: loggerconf: Invalid logger.file.maxBackupFiles: `%s`\n", val);
            nfiles = 0;
        }
        maxBackupFiles = nfiles;
    }
}

LogLevel LoggerConf::parseLevel(const char* s) {
    if (strcmp(s, "TRACE") == 0) {
        return LogLevel_TRACE;
    }
    else if (strcmp(s, "DEBUG") == 0) {
        return LogLevel_DEBUG;
    }
    else if (strcmp(s, "INFO") == 0) {
        return LogLevel_INFO;
    }
    else if (strcmp(s, "WARN") == 0) {
        return LogLevel_WARN;
    }
    else if (strcmp(s, "ERROR") == 0) {
        return LogLevel_ERROR;
    }
    else if (strcmp(s, "FATAL") == 0) {
        return LogLevel_FATAL;
    }
    else {
        fprintf(stderr, "ERROR: loggerconf: Invalid level: `%s`\n", s);
        return logger_getLevel();
    }
}

int LoggerConf::hasFlag(int flags, int flag) {
    return (flags & flag) == flag;
}
