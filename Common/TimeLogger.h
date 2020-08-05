#ifndef TIMELOGGER_H
#define TIMELOGGER_H

#include <QTime>

#include <Shared/LogType.h>

class TimeLogger
{
public:
    TimeLogger(LogTypes logType, const QString& operationName);

    ~TimeLogger();

private:
    const LogTypes logType_;
    const QString operationName_;
    QTime performanceTimer_;
};

#endif  // TIMELOGGER_H
