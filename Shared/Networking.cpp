#include <QNetworkAccessManager>
#include <QtNetwork>
#include <QApplication>

#include "Logger.h"
#include "Networking.h"

const int Networking::maxTries_ = 5;

int Networking::getMaxTries()
{
    return maxTries_;
}

QNetworkRequest Networking::getCurrentVersionRequest()
{
    return QNetworkRequest(QUrl(QLatin1String("http://example.com/update/currentVersion.php")));
}

QNetworkRequest Networking::getDownloadFileRequest(QString file)
{
    return QNetworkRequest(QUrl(QLatin1String("http://example.com/update/current/") + file));
}

bool Networking::errorsOccuredCheck(QNetworkReply* reply)
{
    if (QNetworkReply::NoError != reply->error())
    {
        return true;
    }

    int httpStatusCode =
        reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toUInt();

    return !(httpStatusCode == 200 && reply->isReadable());
}

std::tuple<QString, QStringList> Networking::checkReplyAndReturnAvailableVersion(QNetworkReply* reply)
{
    QString replyString(QLatin1String(reply->readAll()));

    LOG(LOG_NETWORK, QLatin1String("Network reply:\n") + replyString);

    QStringList filesList = replyString.split(QLatin1Char('\n'));

    if (filesList.isEmpty() || filesList.at(0) != QLatin1String("Volbx-Updade-Info"))
    {
        return {QLatin1String(""), filesList};
    }

    LOG(LOG_NETWORK, QLatin1String("Available version is ") + filesList.value(1));

    return {filesList.value(1), filesList};
}
