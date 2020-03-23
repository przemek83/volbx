#include "Constants.h"

#include <cmath>

#include <QLocale>
#include <QSettings>
#include <QVector>

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

const QDate& getStartOfExcelWorld()
{
    static const QDate startOfTheExcelWorld(1899, 12, 30);
    return startOfTheExcelWorld;
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

static QVector<QString> initProgressBarNames()
{
    QVector<QString> progressTitles(static_cast<int>(BarTitle::END));
    progressTitles[static_cast<int>(BarTitle::LOADING)] =
        QObject::tr("Loading");
    progressTitles[static_cast<int>(BarTitle::SAVING)] =
        QObject::tr("Saving");
    progressTitles[static_cast<int>(BarTitle::ANALYSING)] =
        QObject::tr("Analysing");
    return progressTitles;
}

QString getProgressBarTitle(BarTitle barTitle)
{
    static const QVector<QString> progressTitles {initProgressBarNames()};
    return progressTitles[static_cast<int>(barTitle)];
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
    columnNames.reserve(columnsNumber);
    for (int i = 0; i < columnsNumber; ++i)
    {
        columnNames.append(currentPrefix + templateNames[i % templateNames.count()]);

        if (i != 0 && 0 == (i + 1) % templateNames.count())
        {
            currentPrefix = templateNames[i / (templateNames.count() - 1) - 1];
        }
    }

    return columnNames;
}

}  // namespace Constants
