#pragma once

#include <QNetworkRequest>
#include <QString>

class QNetworkReply;

/**
 * Networking related functions shared between binaries.
 */
namespace Networking
{
/**
 * @brief Create network request checking for available version.
 * @return Network request.
 */
QNetworkRequest getCurrentVersionRequest();

/**
 * @brief Create network request for downloading given file.
 * @param file File to download.
 * @return Network request.
 */
QNetworkRequest getDownloadFileRequest(const QString& file);

/**
 * @brief Check if reply is valid.
 * @param reply Network Reply.
 * @return True if valid, false otherwise.
 */
bool replyIsValid(QNetworkReply* reply);

/**
 * @brief Get available version along with available file list.
 * @param reply Reply to parse.
 * @return Available version and file to download list.
 */
std::tuple<QString, QStringList> getAvailableVersionAndFiles(
    QNetworkReply* reply);

/**
 * @brief Get max retires which should be done when connecting.
 * @return Max retries.
 */
int getMaxRetries();
}  // namespace Networking
