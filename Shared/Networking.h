#ifndef NETWORKING_H
#define NETWORKING_H

#include <QString>
#include <QNetworkRequest>

class QNetworkReply;

class Networking
{
public:
    static QNetworkRequest getCurrentVersionRequest();

    static QNetworkRequest getDownloadFileRequest(QString file);

    static bool errorsOccuredCheck(QNetworkReply* reply);

    static QString checkReplyAndReturnAvailableVersion(QNetworkReply* reply,
                                                       QStringList& filesList);

    static int getMaxTries();

private:
    Networking();
    virtual ~Networking();
    Q_DISABLE_COPY(Networking)

    ///Max network tries before reporting error.
    static const int maxTries_;
};

#endif // NETWORKING_H
