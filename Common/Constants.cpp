#include <QSettings>
#include <QLocale>

#include "Constants.h"

const char* Constants::updaterName_ = "VersionCheck";

const char* Constants::exeFileSuffix_ = ".exe";

const char* Constants::tmpFileSuffix_ = ".tmp";

const QDate Constants::startOfTheWorld_ = QDate(1970, 1, 1);

const char* Constants::defaultDateFormat_ = "d/M/yyyy";

const char* Constants::configurationFile_ = "config";

const char* Constants::datasetDefinitionFilename_ = "definition.xml";

const char* Constants::datasetDataFilename_ = "data.csv";

const char* Constants::datasetStringsFilename_ = "strings.txt";

const char* Constants::datasetExtension_ = ".vbx";

const int Constants::minDataPerThread_ = 100;

const char* Constants::datasetNameRegExp_ = "[\\w\\s-]+";

QString Constants::stringFromDays(const int daysToAdd)
{
    return startOfTheWorld_.addDays(daysToAdd).toString(QString(defaultDateFormat_));
}

QString Constants::floatToStringUsingLocale(float value, int precison)
{
    static bool initialized = false;
    static QLocale locale = QLocale::system();
    if (!initialized)
    {
        locale.setNumberOptions(locale.numberOptions() & ~QLocale::OmitGroupSeparator);
        initialized = true;
    }

    return locale.toString(value, 'f', precison);
}

void Constants::generateExcelColumnNames(QStringList& excelColNames,
                                         int columnsNumber)
{
    excelColNames.clear();

    static QStringList templateNames;

    if (templateNames.isEmpty())
    {
        templateNames << "A" << "B" << "C" << "D" << "E" << "F" << "G" << "H" << "I" << "J" <<
                      "K" << "L" << "M" << "N" << "O" << "P" << "Q" << "R" <<
                      "S" << "T" << "U" << "V" << "W" << "X" << "Y" << "Z";
    }

    QString currentPrefix("");
    for (int i = 0; i < columnsNumber; ++i)
    {
        excelColNames << currentPrefix + templateNames[i % templateNames.count()];

        if (i != 0 && 0 == (i + 1) % templateNames.count())
        {
            currentPrefix = templateNames[i / (templateNames.count() - 1) - 1];
        }
    }
}
