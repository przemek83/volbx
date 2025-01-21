#pragma once

#include <QDate>
#include <QString>

class QElapsedTimer;

namespace constants
{
QString getUpdaterName();

QString getExeFileSuffix();

QString getTemporaryFileSuffix();

QString getConfigurationFileName();

enum class BarTitle : signed char
{
    LOADING = 0,
    SAVING,
    ANALYSING,
    END
};

QString getProgressBarTitle(BarTitle barTitle);

const int NOT_SET_COLUMN{-1};

QString elapsedTimeToSeconds(QElapsedTimer timer);

int getProgressBarFullCounter();
};  // namespace constants
