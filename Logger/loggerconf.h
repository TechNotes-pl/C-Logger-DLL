#pragma once
#include <string>
#include "Logger.h"

namespace CLog {

    const auto kMaxFileNameLen = 256;
    const auto kMaxLineLen = 512;

    class  LoggerConf {
    public:
        /**
         * @brief Configure the logger with a configuration file.
         * If the filename is NULL, return without doing anything.
         * @description The following is the configurable key/value list.
         * |key                        |value                                        |
         * |:--------------------------|:--------------------------------------------|
         * |level                      |TRACE, DEBUG, INFO, WARN, ERROR or FATAL     |
         * |autoFlush                  |A flush interval [ms] (off if interval <= 0) |
         * |logger                     |console or file                              |
         * |logger.console.output      |stdout or stderr                             |
         * |logger.file.filename       |A output filename (max length is 255 bytes)  |
         * |logger.file.maxFileSize    |1-LONG_MAX [bytes] (1 MB if size <= 0)       |
         * |logger.file.maxBackupFiles |0-255                                        |
         * 
         * @param[in] filename The name of the configuration file
         * @return Non-zero value upon success or 0 on error
        */
        static int LOGGER_LIBRARY_API logger_configure(const char* filename);

    private:
        // Methods
        static void reset(void);
        static void removeComments(char* s);
        static void trim(char* s);
        static void parseLine(char* line);
        static LogLevel parseLevel(const char* s);
        static int hasFlag(int flags, int flag);

        /**
         * @brief Read configuration file
         * @return 
        */
        std::string read_data_string();
        static std::string trim(const std::string& str, const std::string& whitespace = " \t");
        static std::string reduce(const std::string& str, const std::string& fill = " ", const std::string& whitespace = " \t");

        // Private fields
        inline static int s_logger;
        inline static FILE* m_filepoint;
        inline static char m_filename[kMaxFileNameLen];
        inline static long maxFileSize;
        inline static unsigned char maxBackupFiles;
    };
}