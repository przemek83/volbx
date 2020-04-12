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

QString getDatasetDefinitionFilename();

QString getDatasetDataFilename();

QString getDatasetStringsFilename();

QString getDatasetExtension();

enum class BarTitle : unsigned char
{
    LOADING = 0,
    SAVING,
    ANALYSING,
    END
};

QString getProgressBarTitle(BarTitle barTitle);

/// Minimum amount of data when threading should be used.
static constexpr int minDataPerThread{100};

QString getDatasetNameRegExp();

const int NOT_SET_COLUMN{-1};
};  // namespace Constants

#endif  // CONSTANTS_H
