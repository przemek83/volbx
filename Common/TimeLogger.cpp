#include "TimeLogger.h"

#include "Logger.h"

TimeLogger::TimeLogger(const QString& operationName)
    : operationName_(operationName)
{
    performanceTimer_.start();
}

TimeLogger::~TimeLogger()
{
    LOG(LogTypes::CALC,
        operationName_ + " in " +
            QString::number(performanceTimer_.elapsed() * 1.0 / 1000) +
            " seconds.");
}
