#pragma once

#include <QNetworkAccessManager>
#include <QWidget>

#include "ui_Update.h"

class QNetworkReply;

class Update : public QWidget
{
    Q_OBJECT
public:
    Update();

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    void setupNetworkManagers();

    void connectButtons() const;

    void setupVersions();

    void downloadFile(const QString& fileName);

    void showErrorMsg(const QString& error);

    void fillFilesToUpdateLists(const QStringList& serverInfoList);

    void finalizeUpdate();

    void insertNewSectionIntoDetails(const QString& msg);

    void insertInfoIntoDetails(const QString& msg);

    void insertNewLineIntoDetails();

    void insertSuccessInfoIntoDetails(const QString& msg);

    void insertErrorInfoIntoDetails(const QString& msg);

    void saveVerfiedFile(const QByteArray& fileData, const QString& fileName);

    bool handleVerificationError(const QString& fileName,
                                 const QString& fileSize);

    void startUpdating(const QString& availableVersion,
                       const QStringList& replyStringList);

    void exitUpdaterAsMostRecentVersionIsInstalled();

    bool isReplyOk(const QNetworkReply* reply);

    void renameTempFile(const QString& file);

    Ui::Update ui_;

    QNetworkAccessManager initialInfoNetworkManager_;

    QNetworkAccessManager downloadManager_;

    QVector<QString> filesToDownload_;

    QVector<QString> filesToDownloadSize_;

    QStringList tempFiles_;

    const QString tmpPrefix_{QStringLiteral(".tmp")};

    int currentTriesCount_{0};

private Q_SLOTS:
    void initialInfoNetworkReplyFinished(QNetworkReply* reply);

    void downloadFinished(QNetworkReply* reply);

    void updateProgress(qint64 bytesRead, qint64 totalBytes);

    void showDetailsToggled(bool checked);
};
