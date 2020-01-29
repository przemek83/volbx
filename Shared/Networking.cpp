#include "Networking.h"

#include <QApplication>
#include <QNetworkAccessManager>
#include <QtNetwork>

#include "Logger.h"

const int Networking::maxTries_ = 5;

int Networking::getMaxTries()
{
    return maxTries_;
}

QNetworkRequest Networking::getCurrentVersionRequest()
{
    return QNetworkRequest(QUrl(QStringLiteral("http://example.com/update/currentVersion.php")));
}

QNetworkRequest Networking::getDownloadFileRequest(const QString& file)
{
    return QNetworkRequest(QUrl(QStringLiteral("http://example.com/update/current/") + file));
}

bool Networking::errorsOccuredCheck(QNetworkReply* reply)
{
    if (QNetworkReply::NoError != reply->error())
    {
        return true;
    }

    unsigned int httpStatusCode =
        reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toUInt();

    const int okCode {200};
    return !(httpStatusCode == okCode && reply->isReadable());
}

std::tuple<QString, QStringList> Networking::checkReplyAndReturnAvailableVersion(QNetworkReply* reply)
{
    QString replyString(QLatin1String(reply->readAll()));

    LOG(LogTypes::NETWORK, QLatin1String("Network reply:\n") + replyString);

    QStringList filesList = replyString.split(QLatin1Char('\n'));

    if (filesList.isEmpty() || filesList.at(0) != QLatin1String("Volbx-Updade-Info"))
    {
        return {QLatin1String(""), filesList};
    }

    LOG(LogTypes::NETWORK, QLatin1String("Available version is ") + filesList.value(1));

    return {filesList.value(1), filesList};
}
