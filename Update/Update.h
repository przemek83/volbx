#pragma once

#include <QNetworkAccessManager>
#include <QWidget>

class QNetworkReply;

namespace Ui
{
class Update;
}  // namespace Ui

/**
 * @brief Updater window. Acts as main window for all operations and display.
 */
class Update : public QWidget
{
    Q_OBJECT
public:
    explicit Update(QWidget* parent = nullptr);

    ~Update() override;

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    void downloadFile(const QString& fileName);

    void showErrorMsg(const QString& error);

    /**
     * @brief retrieve info about files to download and sizes.
     * @param serverInfoList files list.
     */
    void fillFilesToUpdateLists(QStringList& serverInfoList);

    /**
     * @brief Rename temporary files.
     */
    void finalizeUpdate();

    void insertNewSectionIntoDetails(const QString& msg);

    void insertInfoIntoDetails(const QString& msg);

    void insertNewLineIntoDetails();

    void insertSuccessInfoIntoDetails(const QString& msg);

    void insertErrorInfoIntoDetails(const QString& msg);

    /**
     * @brief save verified file. Verified means same size as expected.
     * @param fileData file content.
     * @param fileName file name.
     */
    void saveVerfiedFile(QByteArray& fileData, QString& fileName);

    bool handleVerificationError(QString& fileName, QString& fileSize);

    Ui::Update* ui;

    /// Network manager used for getting initial info.
    QNetworkAccessManager initialInfoNetworkManager_;

    /// Network manager used for downloading.
    QNetworkAccessManager downloadManager_;

    /// List of files to download.
    QVector<QString> filesToDownload_;

    /// List of sizes of files to download.
    QVector<QString> filesToDownloadSize_;

    /// List of temporary files.
    QStringList tempFiles_;

    /// Temporary files prefix.
    const QString tmpPrefix_{".tmp"};

    int currentTriesCount_{0};

private Q_SLOTS:
    void initialInfoNetworkReplyFinished(QNetworkReply* reply);

    void downloadFinished(QNetworkReply* reply);

    void updateProgress(qint64 bytesRead, qint64 totalBytes);

    void buttonQuitClicked();

    void showDetailsToggled(bool checked);
};
