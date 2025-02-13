#pragma once

#include <QNetworkRequest>
#include <QString>

class QNetworkReply;

namespace networking
{
QNetworkRequest getCurrentVersionRequest();

QNetworkRequest getDownloadFileRequest(const QString& file);

bool replyIsValid(const QNetworkReply* reply);

std::tuple<QString, QStringList> getAvailableVersionAndFiles(
    QNetworkReply* reply);

int getMaxRetries();
}  // namespace networking
