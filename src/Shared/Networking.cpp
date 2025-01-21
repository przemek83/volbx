#include "Networking.h"

#include <QApplication>
#include <QNetworkAccessManager>
#include <QtNetwork>

#include "Logger.h"

namespace networking
{
int getMaxRetries()
{
    const int maxTries{5};
    return maxTries;
}

QNetworkRequest getCurrentVersionRequest()
{
    return QNetworkRequest(
        QUrl(QStringLiteral("http://example.com/update/currentVersion.php")));
}

QNetworkRequest getDownloadFileRequest(const QString& file)
{
    return QNetworkRequest(
        QUrl(QStringLiteral("http://example.com/update/current/") + file));
}

bool replyIsValid(const QNetworkReply* reply)
{
    if (QNetworkReply::NoError != reply->error())
        return true;

    const int httpStatusCode{
        reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()};

    const int okCode{200};
    return (httpStatusCode != okCode) || (!reply->isReadable());
}

std::tuple<QString, QStringList> getAvailableVersionAndFiles(
    QNetworkReply* reply)
{
    const QString replyString(QLatin1String(reply->readAll()));

    LOG(LogTypes::NETWORK, QStringLiteral("Network reply:\n") + replyString);

    const QStringList filesList{replyString.split(QLatin1Char('\n'))};

    if (filesList.isEmpty() ||
        (filesList.at(0) != QStringLiteral("Volbx-Update-Info")))
        return {QLatin1String(""), filesList};

    LOG(LogTypes::NETWORK,
        QStringLiteral("Available version is ") + filesList.value(1));

    return {filesList.value(1), filesList};
}
}  // namespace networking
