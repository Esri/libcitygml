#pragma once
#include <citygml/citygmllogger.h>
#include <iostream>
class MyLogger : public citygml::CityGMLLogger
{
public:
    MyLogger(LOGLEVEL level = LOGLEVEL::LL_ERROR) : citygml::CityGMLLogger(level) {}
    virtual void log(LOGLEVEL level, const std::string& message, const char* file = nullptr, int line = -1) const override;
};
