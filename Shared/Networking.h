#ifndef NETWORKING_H
#define NETWORKING_H

#include <QNetworkRequest>
#include <QString>

class QNetworkReply;

class Networking final
{
public:
    Networking() = delete;
    ~Networking() = delete;

    Networking& operator=(const Networking& other) = delete;
    Networking(const Networking& other) = delete;

    Networking& operator=(Networking&& other) = delete;
    Networking(Networking&& other) = delete;

    static QNetworkRequest getCurrentVersionRequest();

    static QNetworkRequest getDownloadFileRequest(const QString& file);

    static bool errorsOccuredCheck(QNetworkReply* reply);

    static std::tuple<QString, QStringList> checkReplyAndReturnAvailableVersion(
        QNetworkReply* reply);

    static int getMaxTries();

private:
    /// Max network tries before reporting error.
    static const int maxTries_;
};

#endif  // NETWORKING_H
