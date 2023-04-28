#include <iostream>
#include "Logger.h"

#if defined(_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#define Sleep(n) usleep((n) * 1000)
#endif 

const int kLoggingCount = 10;

int main()
{
    std::cout << "C log example:\n";

    // Console
    InitConsoleLogger(stderr);
    SetLevel(LogLevel_DEBUG);
    LOG_INFO("console logging");
    LOG_INFO("format example: %d%c%s", 1, '2', "3");

    // File
    InitFileLogger("c:/temp/clog.txt");
    SetLevel(LogLevel_DEBUG);
    LOG_INFO("file logging");
    LOG_DEBUG("Simple info  %d", 1);

    LOG_DEBUG("Simple text");
    LOG_DEBUG("Simple debug  %d", 1);
    LOG_DEBUG("format example: %d%c%s", 1, '2', "3");


    InitConsoleLogger(NULL);
    InitFileLogger("c:/temp/clog.txt", 0, 0);
    LOG_INFO("multi logging");
    SetLevel(LogLevel_DEBUG);
    for (int i = 0; i < kLoggingCount; i++) {
        LOG_DEBUG("%d", i);
        Sleep(1000);
    }
}
