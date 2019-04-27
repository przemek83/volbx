#ifndef UPDATE_H
#define UPDATE_H

#include <QWidget>
#include <QNetworkAccessManager>

class QNetworkReply;

namespace Ui
{
class Update;
} // namespace Ui

/**
 * @brief Updater window. Acts as main window for all operations and display.
 */
class Update : public QWidget
{
    Q_OBJECT
public:
    explicit Update(QWidget* parent = nullptr);

    ~Update() override;

    Update& operator=(const Update& other) = delete;
    Update(const Update& other) = delete;

    Update& operator=(Update&& other) = delete;
    Update(Update&& other) = delete;

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    Ui::Update* ui;

    ///Network manager used for getting initial info.
    QNetworkAccessManager initialInfoNetworkManager_;

    ///Network manager used for downloading.
    QNetworkAccessManager downloadManager_;

    void downloadFile(const QString& fileName);

    ///List of files to download.
    QVector<QString> filesToDownload_;

    ///List of sizes of files to download.
    QVector<QString> filesToDownloadSize_;

    ///List of temporary files.
    QStringList tempFiles_;

    ///Temporary files prefix.
    static const char* tmpPrefix_;

    int currentTriesCount_ {0};

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
     * @brief save verfied file. Verified mean of same size as expected.
     * @param fileData file content.
     * @param fileName file name.
     */
    void saveVerfiedFile(QByteArray& fileData, QString& fileName);

    bool handleVerificationError(QString& fileName, QString& fileSize);

private Q_SLOTS:
    void initialInfoNetworkReplyFinished(QNetworkReply* reply);

    void downloadFinished(QNetworkReply* reply);

    void updateProgress(qint64 bytesRead, qint64 totalBytes);

    void on_buttonQuit_clicked();

    void on_showDetails_toggled(bool checked);
};

#endif // UPDATE_H
