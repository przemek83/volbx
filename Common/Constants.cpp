#include "Constants.h"

#include <cmath>

#include <QElapsedTimer>
#include <QLocale>
#include <QSettings>
#include <QVector>

namespace Constants
{
QString getUpdaterName() { return QStringLiteral("VersionCheck"); }

QString getExeFileSuffix() { return QStringLiteral(".exe"); }

QString getTemporaryFileSuffix() { return QStringLiteral(".tmp"); }

QString getConfigurationFileName() { return QStringLiteral("config"); }

QString getProgressBarTitle(BarTitle barTitle)
{
    const QVector<QString> progressTitles{QObject::tr("Loading"),
                                          QObject::tr("Saving"),
                                          QObject::tr("Analysing")};
    return progressTitles[static_cast<int>(barTitle)];
}

QString timeFromTimeToSeconds(QElapsedTimer time)
{
    const int milisecondsInSecond{1000};
    return QString::number(static_cast<double>(time.elapsed()) /
                           milisecondsInSecond);
}

int getProgressBarFullCounter()
{
    const int fullCounter{100};
    return fullCounter;
}

}  // namespace Constants
