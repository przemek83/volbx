#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QDate>
#include <QString>

namespace Constants
{

QString getUpdaterName();

QString getExeFileSuffix();

QString getTemporaryFileSuffix();

const QDate& getStartOfExcelWorld();

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

///Minimum amount of data when threading should be used.
static constexpr int minDataPerThread{100};

QString getDatasetNameRegExp();

///String regexp for field used to input dataset name.
QStringList generateExcelColumnNames(int columnsNumber);

static constexpr int MAX_EXCEL_COLUMNS {600};

const int NOT_SET_COLUMN {-1};
};

#endif // CONSTANTS_H
