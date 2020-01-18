#include "Constants.h"

#include <cmath>

#include <QLocale>
#include <QSettings>

namespace Constants
{
QString stringFromDays(int daysToAdd)
{
    return startOfTheWorld.addDays(daysToAdd).toString(defaultDateFormat);
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
    return std::abs(left - right) <= 1e-12 * std::max({ 1.0, std::abs(left), std::abs(right) });
}

bool floatsAreEqual(float left, float right)
{
    return std::abs(left - right) <= 1e-6F * std::max({ 1.0F, std::abs(left), std::abs(right) });
}
}  // namespace Constants
