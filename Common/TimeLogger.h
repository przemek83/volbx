#ifndef TIMELOGGER_H
#define TIMELOGGER_H

#include <QTime>

class TimeLogger
{
public:
    explicit TimeLogger(const QString& operationName);

    ~TimeLogger();

private:
    const QString operationName_;
    QTime performanceTimer_;
};

#endif  // TIMELOGGER_H
