#include <iostream>
#include "Logger.h"

template <typename T, typename U>
constexpr auto MAX(T a, U b) { return (((a) > (b)) ? (a) : (b)); }

int main()
{
    std::cout << "Hello World!\n";

    // Console 
    InitConsoleLogger(stderr);
    SetLevel(LogLevel_DEBUG);
    LOG_INFO("console logging");
    LOG_INFO("format example: %d%c%s", 1, '2', "3");

    InitFileLogger("log.txt", 1024 * 1024, 5);
    SetLevel(LogLevel_DEBUG);
    LOG_INFO("file logging");
    LOG_DEBUG("Simple info  %d", 1);

    LOG_DEBUG("Simple text");
    LOG_DEBUG("Simple debug  %d", 1);
    LOG_DEBUG("format example: %d%c%s", 1, '2', "3");
}
