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

QString Constants::stringFromDays(int daysToAdd)
{
    return startOfTheWorld_.addDays(daysToAdd).toString(defaultDateFormat_);
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

QStringList Constants::generateExcelColumnNames(int columnsNumber)
{
    QStringList columnNames;

    static QStringList templateNames;

    if (templateNames.isEmpty())
    {
        templateNames << QStringLiteral("A") <<
                      QStringLiteral("B") << QStringLiteral("C") << QStringLiteral("D") <<
                      QStringLiteral("E") << QStringLiteral("F") << QStringLiteral("G") <<
                      QStringLiteral("H") << QStringLiteral("I") << QStringLiteral("J") <<
                      QStringLiteral("K") << QStringLiteral("L") << QStringLiteral("M") <<
                      QStringLiteral("N") << QStringLiteral("O") << QStringLiteral("P") <<
                      QStringLiteral("Q") << QStringLiteral("R") << QStringLiteral("S") <<
                      QStringLiteral("T") << QStringLiteral("U") << QStringLiteral("V") <<
                      QStringLiteral("W") << QStringLiteral("X") << QStringLiteral("Y") <<
                      QStringLiteral("Z");
    }

    QString currentPrefix(QLatin1String(""));
    for (int i = 0; i < columnsNumber; ++i)
    {
        columnNames << currentPrefix + templateNames[i % templateNames.count()];

        if (i != 0 && 0 == (i + 1) % templateNames.count())
        {
            currentPrefix = templateNames[i / (templateNames.count() - 1) - 1];
        }
    }

    return columnNames;
}
