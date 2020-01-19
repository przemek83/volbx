#include "Constants.h"

#include <cmath>

#include <QLocale>
#include <QSettings>

namespace Constants
{

QString getUpdaterName()
{
    return QStringLiteral("VersionCheck");
}

QString getExeFileSuffix()
{
    return QStringLiteral(".exe");
}

QString getTemporaryFileSuffix()
{
    return QStringLiteral(".tmp");
}

QDate getStartOfTheWorld()
{
    static const QDate startOfTheWorld{QDate(1970, 1, 1)};
    return startOfTheWorld;
}

QString getDefaultDateFormat()
{
    return QStringLiteral("d/M/yyyy");
}

QString getConfigurationFileName()
{
    return QStringLiteral("config");
}

QString getDatasetDefinitionFilename()
{
    return QStringLiteral("definition.xml");
}

QString getDatasetDataFilename()
{
    return QStringLiteral("data.csv");
}

QString getDatasetStringsFilename()
{
    return QStringLiteral("strings.txt");
}

QString getDatasetExtension()
{
    return QStringLiteral(".vbx");
}

QString stringFromDays(int daysToAdd)
{
    return getStartOfTheWorld().addDays(daysToAdd).toString(getDefaultDateFormat());
}

QString floatToStringUsingLocale(float value, int precison)
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

QString getDatasetNameRegExp()
{
    return QStringLiteral("[\\w\\s-]+");
}

QStringList generateExcelColumnNames(int columnsNumber)
{
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
    QStringList columnNames;
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

bool doublesAreEqual(double left, double right)
{
    static const double qtDoublePrecision {1e-12};
    return std::abs(left - right) <=
           qtDoublePrecision * std::max({ 1.0, std::abs(left), std::abs(right) });
}

bool floatsAreEqual(float left, float right)
{
    static const float qtFloatPrecision {1e-6F};
    return std::abs(left - right) <=
           qtFloatPrecision * std::max({ 1.0F, std::abs(left), std::abs(right) });
}
}  // namespace Constants
