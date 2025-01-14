#pragma once

#include <QDate>
#include <QString>

class QElapsedTimer;

namespace Constants
{
QString getUpdaterName();

QString getExeFileSuffix();

QString getTemporaryFileSuffix();

QString getConfigurationFileName();

enum class BarTitle : unsigned char
{
    LOADING = 0,
    SAVING,
    ANALYSING,
    END
};

QString getProgressBarTitle(BarTitle barTitle);

const int NOT_SET_COLUMN{-1};

QString timeFromTimeToSeconds(QElapsedTimer time);

int getProgressBarFullCounter();
};  // namespace Constants
