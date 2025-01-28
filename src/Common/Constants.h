#pragma once

#include <QString>

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

QString elapsedTimeToSeconds(qint64 elapsedMiliseconds);

int getProgressBarFullCounter();
};  // namespace constants
