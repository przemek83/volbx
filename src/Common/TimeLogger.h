#pragma once

#include <QElapsedTimer>
#include <QTime>

#include <Shared/LogType.h>

/**
 * @brief Automatic time logger for measuring time of execution.
 */
class TimeLogger
{
public:
    TimeLogger(LogTypes logType, QString operationName);

    ~TimeLogger();

    TimeLogger& operator=(const TimeLogger& other) = delete;
    TimeLogger(const TimeLogger& other) = delete;

    TimeLogger& operator=(TimeLogger&& other) = delete;
    TimeLogger(TimeLogger&& other) = delete;

private:
    const LogTypes logType_;
    const QString operationName_;
    QElapsedTimer performanceTimer_;
};
