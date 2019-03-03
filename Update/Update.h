#ifndef UPDATE_H
#define UPDATE_H

#include <QWidget>
#include <QNetworkAccessManager>

class QNetworkReply;

namespace Ui {
    class Update;
}

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
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::Update *ui;

    ///Network manager used for getting initial info.
    QNetworkAccessManager initialInfoNetworkManager_;

    ///Network manager used for downloading.
    QNetworkAccessManager downloadManager_;

    void downloadFile(QString fileName);

    ///List of files to download.
    QVector<QString> filesToDownload_;

    ///List of sizes of files to download.
    QVector<QString> filesToDownloadSize_;

    ///List of temporary files.
    QStringList tempFiles_;

    ///Temporary files prefix.
    static const char* tmpPrefix_;

    int currentTriesCount_;

    void showErrorMsg(QString error);

    /**
     * @brief retrieve info about files to download and sizes.
     * @param serverInfoList files list.
     */
    void fillFilesToUpdateLists(QStringList& serverInfoList);

    /**
     * @brief Rename temporary files.
     */
    void finalizeUpdate();

    void insertNewSectionIntoDetails(QString msg);

    void insertInfoIntoDetails(QString msg);

    void insertNewLineIntoDetails();

    void insertSuccessInfoIntoDetails(QString msg);

    void insertErrorInfoIntoDetails(QString msg);

    /**
     * @brief save verfied file. Verified mean of same size as expected.
     * @param fileData file content.
     * @param fileName file name.
     */
    void saveVerfiedFile(QByteArray& fileData, QString& fileName);

    bool handleVerificationError(QString& fileName, QString& fileSize);

private slots:
    void initialInfoNetworkReplyFinished(QNetworkReply* reply);

    void downloadFinished(QNetworkReply* reply);

    void updateProgress(qint64 bytesRead, qint64 totalBytes);

    void on_buttonQuit_clicked();

    void on_showDetails_toggled(bool checked);
};

#endif // UPDATE_H
