#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QDate>
#include <QString>

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
};  // namespace Constants

#endif  // CONSTANTS_H
