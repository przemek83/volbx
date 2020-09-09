#include "Constants.h"

#include <cmath>

#include <QLocale>
#include <QSettings>
#include <QTime>
#include <QVector>

namespace Constants
{
QString getUpdaterName() { return QStringLiteral("VersionCheck"); }

QString getExeFileSuffix() { return QStringLiteral(".exe"); }

QString getTemporaryFileSuffix() { return QStringLiteral(".tmp"); }

QString getConfigurationFileName() { return QStringLiteral("config"); }

static QVector<QString> initProgressBarNames()
{
    QVector<QString> progressTitles(static_cast<int>(BarTitle::END));
    progressTitles[static_cast<int>(BarTitle::LOADING)] =
        QObject::tr("Loading");
    progressTitles[static_cast<int>(BarTitle::SAVING)] = QObject::tr("Saving");
    progressTitles[static_cast<int>(BarTitle::ANALYSING)] =
        QObject::tr("Analysing");
    return progressTitles;
}

QString getProgressBarTitle(BarTitle barTitle)
{
    static const QVector<QString> progressTitles{initProgressBarNames()};
    return progressTitles[static_cast<int>(barTitle)];
}

QString timeFromTimeToSeconds(QTime time)
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
