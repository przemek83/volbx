#include "TimeLogger.h"

#include "Constants.h"
#include "Logger.h"

TimeLogger::TimeLogger(LogTypes logType, QString operationName)
    : logType_(logType), operationName_(std::move(operationName))
{
    performanceTimer_.start();
}

TimeLogger::~TimeLogger()
{
    const QString seconds{constants::timeFromTimeToSeconds(performanceTimer_)};
    LOG(logType_, operationName_ + " in " + seconds + " seconds.");
}
