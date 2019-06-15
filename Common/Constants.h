#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QDate>
#include <QString>

/**
 * @brief Wrapper class containing lots of constants used in different part of code.
 */
struct Constants
{
public:
    Constants() = delete;
    ~Constants() = delete;

    Constants& operator=(const Constants& other) = delete;
    Constants(const Constants& other) = delete;

    Constants& operator=(Constants&& other) = delete;
    Constants(Constants&& other) = delete;

    ///Name of updater binary.
    static const char* updaterName_;

    ///Exe file extension.
    static const char* exeFileSuffix_;

    ///Temporary file extension.
    static const char* tmpFileSuffix_;

    ///Date 1/1/1970 used as reference for dates conversion.
    static const QDate startOfTheWorld_;

    ///Default dates format.
    static const char* defaultDateFormat_;

    ///Name of config file.
    static const char* configurationFile_;

    ///Extension of dataset definition files.
    static const char* datasetDefinitionFilename_;

    ///Extension of dataset data files.
    static const char* datasetDataFilename_;

    ///Extension of dataset strings files.
    static const char* datasetStringsFilename_;

    ///Extension of dataset file.
    static const char* datasetExtension_;

    /**
     * Calculate date 1/1/1970 + given days and return string.
     * @param daysToAdd amount of days to add.
     * @return date in for of string.
     **/
    static QString stringFromDays(int daysToAdd);

    ///Minimum amount of data when threading should be used.
    static const int minDataPerThread_;

    /**
     * Visualize given number as string using localization options/format.
     * @param value number to visualize.
     * @param precison precision for visualization.
     * @return number as string conveted using locale.
     **/
    static QString floatToStringUsingLocale(float value, int precison);

    ///String regexp for field used to input dataset name.
    static const char* datasetNameRegExp_;

    static QStringList generateExcelColumnNames(int columnsNumber);
};

#endif // CONSTANTS_H
