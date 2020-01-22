#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QDate>
#include <QString>

namespace Constants
{

QString getUpdaterName();

QString getExeFileSuffix();

QString getTemporaryFileSuffix();

///Date 1/1/1970 used as reference for dates conversion.
QDate getStartOfTheWorld();

QString getDefaultDateFormat();

QString getConfigurationFileName();

QString getDatasetDefinitionFilename();

QString getDatasetDataFilename();

QString getDatasetStringsFilename();

QString getDatasetExtension();

/**
 * Calculate date 1/1/1970 + given days and return string.
 * @param daysToAdd amount of days to add.
 * @return date in for of string.
 **/
QString stringFromDays(int daysToAdd);

///Minimum amount of data when threading should be used.
static constexpr int minDataPerThread{100};

/**
 * Visualize given number as string using localization options/format.
 * @param value number to visualize.
 * @param precison precision for visualization.
 * @return number as string conveted using locale.
 **/
QString floatToStringUsingLocale(float value, int precison);

QString getDatasetNameRegExp();

///String regexp for field used to input dataset name.
QStringList generateExcelColumnNames(int columnsNumber);

/// Values used to enforce drawing order of items on plots.
static constexpr int LOW_ORDER{1};
static constexpr int MEDIUM_ORDER{2};
static constexpr int HIGH_ORDER{3};

static constexpr int BIG_TRANSPARENCY_FACTOR {50};
static constexpr int SMALL_TRANSPARENCY_FACTOR {200};

bool doublesAreEqual(double left, double right);
bool floatsAreEqual(float left, float right);

static constexpr double DEFAULT_LABEL_ROTATION {-50.0};

static constexpr int MAX_EXCEL_COLUMNS {600};

const int NOT_SET_COLUMN {-1};
};

#endif // CONSTANTS_H
