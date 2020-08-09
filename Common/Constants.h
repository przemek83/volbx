#pragma once

#include <QDate>
#include <QString>

/**
 * Helper functions for constants.
 */
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
