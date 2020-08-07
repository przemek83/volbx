#ifndef TIMELOGGER_H
#define TIMELOGGER_H

#include <QTime>

#include <Shared/LogType.h>

/**
 * @brief Automatic time logger for mesauring time of execution.
 */
class TimeLogger
{
public:
    TimeLogger(LogTypes logType, const QString& operationName);

    ~TimeLogger();

    TimeLogger& operator=(const TimeLogger& other) = delete;
    TimeLogger(const TimeLogger& other) = delete;

    TimeLogger& operator=(TimeLogger&& other) = delete;
    TimeLogger(TimeLogger&& other) = delete;

private:
    const LogTypes logType_;
    const QString operationName_;
    QTime performanceTimer_;
};

#endif  // TIMELOGGER_H
