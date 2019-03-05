#include <QNetworkRequest>
#include <QFile>
#include <QNetworkReply>
#include <QMessageBox>
#include <QDebug>

#include "Shared/Logger.h"
#include "Shared/Networking.h"

#include "Update.h"
#include "ui_Update.h"

const char* Update::tmpPrefix_ = ".tmp";

Update::Update(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::Update)
{
    ui->setupUi(this);

    setWindowFlags(Qt::CustomizeWindowHint |
                   Qt::WindowTitleHint |
                   Qt::WindowCloseButtonHint);

    ui->valueAvailable->setText(tr("checking..."));
    ui->valueActual->setText(QApplication::applicationVersion());

    //Attach proper slots to signals.
    connect(&initialInfoNetworkManager_,
            SIGNAL(finished(QNetworkReply*)),
            this,
            SLOT(initialInfoNetworkReplyFinished(QNetworkReply*)));

    connect(&downloadManager_,
            SIGNAL(finished(QNetworkReply*)),
            this,
            SLOT(downloadFinished(QNetworkReply*)));

    //Get available version and files to update list.
    initialInfoNetworkManager_.get(Networking::getCurrentVersionRequest());

    insertInfoIntoDetails(tr("Connecting to update server") + QLatin1String("... "));

    LOG(LOG_NETWORK, QLatin1String("Initial network request send."));

    ui->details->hide();
}

Update::~Update()
{
    delete ui;
}

void Update::initialInfoNetworkReplyFinished(QNetworkReply* reply)
{
    reply->deleteLater();

    //Check errors.
    if (Networking::errorsOccuredCheck(reply))
    {
        insertNewLineIntoDetails();
        insertErrorInfoIntoDetails(tr("Error") +
                                   QLatin1Char(':') + reply->errorString());
        showErrorMsg(tr("Connection error encountered."));
        return;
    }

    insertSuccessInfoIntoDetails(tr("Connected"));
    insertNewLineIntoDetails();
    insertNewLineIntoDetails();

    auto [newestVersion, replyStringList] =
        Networking::checkReplyAndReturnAvailableVersion(reply);

    if (newestVersion.isEmpty())
    {
        showErrorMsg(tr("Wrong answer received from server."));
        return;
    }

    if (newestVersion != QApplication::applicationVersion())
    {
        ui->valueAvailable->setText(newestVersion);

        fillFilesToUpdateLists(replyStringList);

        if (0 == filesToDownload_.count() ||
            0 == filesToDownloadSize_.count() ||
            filesToDownload_.count() != filesToDownloadSize_.count())
        {
            showErrorMsg(tr("Wrong data received from update server."));
            return;
        }

        ui->fileCount->setText(QLatin1Char('/') +
                               QString::number(filesToDownload_.count()));

        insertNewLineIntoDetails();
        insertNewSectionIntoDetails(tr("Downloading files") + QLatin1Char(':'));

        downloadFile(filesToDownload_.front());
    }
    else
    {
        QMessageBox::information(this,
                                 tr("No update needed"),
                                 tr("Application is up to date."));
        close();
    }
}

void Update::fillFilesToUpdateLists(QStringList& serverInfoList)
{
    //First line/element is correctness checker, second version.
    int filesCount = serverInfoList.size() - 2;

    insertNewSectionIntoDetails(tr("Found") + QLatin1Char(' ') +
                                QString::number(filesCount) + QLatin1Char(' ') +
                                (filesCount != 1 ? tr("files") : tr("file")) +
                                QLatin1Char(' ') + tr("to update") + QLatin1Char(':'));

    filesToDownload_.clear();
    filesToDownloadSize_.clear();
    for (int i = 0; i < filesCount; ++i)
    {
        QString fileInfoLine = serverInfoList.at(2 + i);
        QString fileName = fileInfoLine.section(QLatin1Char(';'), 0, 0);
        QString fileSize = fileInfoLine.section(QLatin1Char(';'), 1);
        filesToDownload_.push_back(fileName);
        filesToDownloadSize_.push_back(fileSize);

        insertInfoIntoDetails(QString::number(i + 1) + QLatin1String(". ") + fileName +
                              QLatin1String(" (") + tr("size") + QLatin1String(": ") +
                              fileSize + QLatin1Char(' ') + tr("bytes") +
                              QLatin1Char(')'));
        insertNewLineIntoDetails();
    }
}

void Update::downloadFile(QString fileName)
{
    insertInfoIntoDetails(fileName + QLatin1String("... "));

    //Update number of downloaded file only when previous was downloaded correctly.
    if (0 == currentTriesCount_)
    {
        ui->currentFile->setText(QString::number(ui->currentFile->text().toInt() + 1));
    }

    QNetworkReply* reply =
        downloadManager_.get(Networking::getDownloadFileRequest(fileName));

    LOG(LOG_NETWORK,
        QLatin1String("Sent request for downloading file ") + QString(fileName));

    ui->progressBar->reset();

    connect(reply, SIGNAL(downloadProgress(qint64, qint64)),
            this, SLOT(updateProgress(qint64, qint64)));
}

void Update::downloadFinished(QNetworkReply* reply)
{
    reply->deleteLater();

    if (Networking::errorsOccuredCheck(reply))
    {
        insertNewLineIntoDetails();
        insertErrorInfoIntoDetails(tr("Error") +
                                   QLatin1Char(':') + reply->errorString());
        showErrorMsg(tr("Connection error encountered."));
        return;
    }

    insertSuccessInfoIntoDetails(tr("Downloaded"));
    insertInfoIntoDetails(QLatin1String("... "));

    QString fileName(filesToDownload_.takeFirst());
    QString fileSize(filesToDownloadSize_.takeFirst());

    QByteArray fileDownloadedContent = reply->readAll();

    LOG(LOG_NETWORK,
        QLatin1String("Expected file size ") + QString::number(fileSize.toInt()) +
        QLatin1String(", size of downloaded file ") +
        QString::number(fileDownloadedContent.size()) + QLatin1Char('.'));

    //Verification of file size.
    if (fileDownloadedContent.size() == fileSize.toInt())
    {
        saveVerfiedFile(fileDownloadedContent, fileName);
    }
    else
    {
        //Do not continue if max tries reached.
        if (!handleVerificationError(fileName, fileSize))
        {
            return;
        }
    }

    if (filesToDownload_.isEmpty())
    {
        finalizeUpdate();

        close();

        return;
    }

    downloadFile(filesToDownload_.front());
}

void Update::saveVerfiedFile(QByteArray& fileData, QString& fileName)
{
    QFile file(QApplication::applicationDirPath() + QLatin1Char('/') +
               fileName + QLatin1String(tmpPrefix_));
    insertSuccessInfoIntoDetails(tr("Verified"));
    insertNewLineIntoDetails();
    file.remove();
    file.open(QIODevice::WriteOnly);
    file.write(fileData);

    LOG(LOG_NETWORK,
        QLatin1String("Saved file: ") + fileName + QLatin1String(tmpPrefix_));

    file.close();
    tempFiles_.push_back(file.fileName());
    currentTriesCount_ = 0;
}

bool Update::handleVerificationError(QString& fileName, QString& fileSize)
{
    insertErrorInfoIntoDetails(tr("Error during verification!"));

    //Add corrupted file to front of vector if max tries not reached yet.
    if (currentTriesCount_ >= Networking::getMaxTries())
    {
        insertErrorInfoIntoDetails(tr("Can not download file."));
        QString msg(tr("Updating interrupted - downloading error. Please retry later."));
        QMessageBox::critical(this,
                              tr("Updating interrupted"),
                              msg);
        return false;

    }

    filesToDownload_.push_front(fileName);
    filesToDownloadSize_.push_front(fileSize);
    currentTriesCount_++;

    return true;
}

void Update::finalizeUpdate()
{
    insertNewLineIntoDetails();
    insertNewSectionIntoDetails(tr("Renaming temporary filenames") + QLatin1Char(':'));
    for (const QString& tempFileName : tempFiles_)
    {
        QString targetFileName(tempFileName);
        targetFileName.chop(QLatin1String(tmpPrefix_).size());

        insertInfoIntoDetails(tempFileName.section(QLatin1Char('/'), -1) +
                              QLatin1String(" -> ") +
                              targetFileName.section(QLatin1Char('/'), -1));
        insertNewLineIntoDetails();

        QFile::remove(targetFileName);
        QFile::rename(tempFileName, targetFileName);
    }

    insertNewLineIntoDetails();
    insertSuccessInfoIntoDetails(tr("Update complete") + QLatin1Char('.'));
    insertNewLineIntoDetails();

    ui->valueActual->setText(ui->valueAvailable->text());

    QMessageBox::information(nullptr,
                             tr("Update complete"),
                             tr("Application update is completed."));
}

void Update::updateProgress(qint64 bytesRead, qint64 totalBytes)
{
    ui->progressBar->setMaximum(totalBytes);
    ui->progressBar->setValue(bytesRead);
}

void Update::closeEvent(QCloseEvent* event)
{
    QWidget::closeEvent(event);

    //If logger window is shown closing mainWindow do not close app.
    QApplication::closeAllWindows();
}

void Update::on_buttonQuit_clicked()
{
    close();
}

void Update::showErrorMsg(QString error)
{
    if (!ui->details->isVisible())
    {
        ui->showDetails->setChecked(true);
    }

    //Do not close app. Allow user to check details.
    QMessageBox::critical(this, tr("Error"), error);
}

void Update::insertNewSectionIntoDetails(QString msg)
{
    ui->details->insertHtml(QLatin1String("<b><FONT COLOR=blue>") +
                            msg +
                            QLatin1String("</FONT></b><br>"));
    ui->details->ensureCursorVisible();
}

void Update::insertInfoIntoDetails(QString msg)
{
    ui->details->insertHtml(QLatin1String("<FONT COLOR=black>") +
                            msg +
                            QLatin1String("</FONT>"));
    ui->details->ensureCursorVisible();
}

void Update::insertNewLineIntoDetails()
{
    ui->details->insertHtml(QLatin1String("<br>"));
    ui->details->ensureCursorVisible();
}

void Update::insertSuccessInfoIntoDetails(QString msg)
{
    ui->details->insertHtml(QLatin1String("<b><FONT COLOR=green>") +
                            msg +
                            QLatin1String("</FONT></b>"));
    ui->details->ensureCursorVisible();
}

void Update::insertErrorInfoIntoDetails(QString msg)
{
    ui->details->insertHtml(QLatin1String("<b><FONT COLOR=red>") +
                            msg +
                            QLatin1String("</FONT></b>"));
    ui->details->ensureCursorVisible();
}


void Update::on_showDetails_toggled(bool checked)
{
    static int detailsSize = 0;
    if (checked)
    {
        int minimumSize = ui->verticalLayout->minimumSize().height();
        ui->details->show();
        setMinimumHeight(minimumSize + ui->details->minimumHeight());
        resize(width(), minimumSize + detailsSize);
    }
    else
    {
        detailsSize = ui->details->height();
        ui->details->hide();
        int minimumSize = ui->verticalLayout->minimumSize().height();
        setMinimumHeight(minimumSize);
    }
}
