#include "MyLogger.h"

void MyLogger::log(LOGLEVEL level, const std::string& message, const char* file, int line) const
{
    std::cout << message << std::endl;
}