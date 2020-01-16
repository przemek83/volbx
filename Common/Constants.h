#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QDate>
#include <QString>

namespace Constants
{
///Name of updater binary.
static const char* updaterName{"VersionCheck"};

///Exe file extension.
static const char* exeFileSuffix{".exe"};

///Temporary file extension.
static const char* tmpFileSuffix{".tmp"};

///Date 1/1/1970 used as reference for dates conversion.
static const QDate startOfTheWorld{QDate(1970, 1, 1)};

///Default dates format.
static const char* defaultDateFormat{"d/M/yyyy"};

///Name of config file.
static const char* configurationFile{"config"};

///Extension of dataset definition files.
static const char* datasetDefinitionFilename{"definition.xml"};

///Extension of dataset data files.
static const char* datasetDataFilename{"data.csv"};

///Extension of dataset strings files.
static const char* datasetStringsFilename{"strings.txt"};

///Extension of dataset file.
static const char* datasetExtension{".vbx"};

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

///String regexp for field used to input dataset name.
static const char* datasetNameRegExp{"[\\w\\s-]+"};

QStringList generateExcelColumnNames(int columnsNumber);

/// Values used to enforce drawing order of items on plots.
static constexpr int LOW_ORDER{1};
static constexpr int MEDIUM_ORDER{2};
static constexpr int HIGH_ORDER{3};
};

#endif // CONSTANTS_H
