#include "Constants.h"

#include <QObject>

namespace constants
{
QString getUpdaterName() { return QStringLiteral("VersionCheck"); }

QString getExeFileSuffix() { return QStringLiteral(".exe"); }

QString getTemporaryFileSuffix() { return QStringLiteral(".tmp"); }

QString getConfigurationFileName() { return QStringLiteral("config"); }

QString getProgressBarTitle(BarTitle barTitle)
{
    if (barTitle == BarTitle::LOADING)
        return QObject::tr("Loading");

    if (barTitle == BarTitle::SAVING)
        return QObject::tr("Saving");

    return QObject::tr("Analysing");
}

QString elapsedTimeToSeconds(qint64 elapsedMiliseconds)
{
    const int milisecondsInSecond{1000};
    return QString::number(std::round(static_cast<double>(elapsedMiliseconds) /
                                      milisecondsInSecond));
}

int getProgressBarFullCounter()
{
    const int fullCounter{100};
    return fullCounter;
}

}  // namespace constants
