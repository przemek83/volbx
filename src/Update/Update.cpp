#include "Update.h"

#include <QFile>
#include <QMessageBox>
#include <QNetworkReply>

#include <Shared/Logger.h>
#include <Shared/Networking.h>

Update::Update() : QWidget()
{
    ui_.setupUi(this);

    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint |
                   Qt::WindowCloseButtonHint);

    setupVersions();
    setupNetworkManagers();
    connectButtons();

    ui_.details->hide();
}

void Update::setupNetworkManagers()
{
    connect(&initialInfoNetworkManager_, &QNetworkAccessManager::finished, this,
            &Update::initialInfoNetworkReplyFinished);

    connect(&downloadManager_, &QNetworkAccessManager::finished, this,
            &Update::downloadFinished);

    insertInfoIntoDetails(tr("Connecting to update server") +
                          QStringLiteral("... "));

    // Get available version and files to update list.
    initialInfoNetworkManager_.get(Networking::getCurrentVersionRequest());

    LOG(LogTypes::NETWORK, QStringLiteral("Initial network request send."));
}

void Update::connectButtons() const
{
    connect(ui_.buttonQuit, &QPushButton::clicked, this, &Update::close);
    connect(ui_.showDetails, &QCheckBox::toggled, this,
            &Update::showDetailsToggled);
}

void Update::setupVersions()
{
    ui_.valueAvailable->setText(tr("checking..."));
    ui_.valueActual->setText(QApplication::applicationVersion());
}

void Update::startUpdating(const QString& availableVersion,
                           const QStringList& replyStringList)
{
    ui_.valueAvailable->setText(availableVersion);

    fillFilesToUpdateLists(replyStringList);

    if (filesToDownload_.isEmpty() || filesToDownloadSize_.isEmpty() ||
        filesToDownload_.count() != filesToDownloadSize_.count())
    {
        showErrorMsg(tr("Wrong data received from update server."));
        return;
    }

    ui_.fileCount->setText(QLatin1Char('/') +
                           QString::number(filesToDownload_.count()));

    insertNewLineIntoDetails();
    insertNewSectionIntoDetails(tr("Downloading files") + QLatin1Char(':'));

    downloadFile(filesToDownload_.constFirst());
}

void Update::exitUpdaterAsMostRecentVersionIsInstalled()
{
    QMessageBox::information(this, tr("No update needed"),
                             tr("Application is up to date."));
    close();
}

bool Update::isReplyOk(const QNetworkReply* reply)
{
    if (!Networking::replyIsValid(reply))
    {
        insertNewLineIntoDetails();
        insertErrorInfoIntoDetails(tr("Error") + QLatin1Char(':') +
                                   reply->errorString());
        showErrorMsg(tr("Connection error encountered."));
        return false;
    }
    return true;
}

void Update::initialInfoNetworkReplyFinished(QNetworkReply* reply)
{
    reply->deleteLater();
    if (!isReplyOk(reply))
        return;

    insertSuccessInfoIntoDetails(tr("Connected"));
    insertNewLineIntoDetails();
    insertNewLineIntoDetails();

    auto [availableVersion,
          replyStringList]{Networking::getAvailableVersionAndFiles(reply)};
    if (availableVersion.isEmpty())
    {
        showErrorMsg(tr("Wrong answer received from server."));
        return;
    }

    if (QApplication::applicationVersion() != availableVersion)
        startUpdating(availableVersion, replyStringList);
    else
        exitUpdaterAsMostRecentVersionIsInstalled();
}

void Update::fillFilesToUpdateLists(const QStringList& serverInfoList)
{
    // First line/element is correctness checker, second version.
    const qsizetype filesCount{serverInfoList.size() - 2};
    insertNewSectionIntoDetails(
        tr("Found") + QLatin1Char(' ') + QString::number(filesCount) +
        QLatin1Char(' ') + (filesCount != 1 ? tr("files") : tr("file")) +
        QLatin1Char(' ') + tr("to update") + QLatin1Char(':'));

    filesToDownload_.clear();
    filesToDownloadSize_.clear();
    for (int i{0}; i < filesCount; ++i)
    {
        const QString& fileInfoLine{serverInfoList.at(2 + i)};
        const QString fileName{fileInfoLine.section(QLatin1Char(';'), 0, 0)};
        const QString fileSize{fileInfoLine.section(QLatin1Char(';'), 1)};
        filesToDownload_.push_back(fileName);
        filesToDownloadSize_.push_back(fileSize);

        insertInfoIntoDetails(
            QString::number(i + 1) + QStringLiteral(". ") + fileName +
            QStringLiteral(" (") + tr("size") + QStringLiteral(": ") +
            fileSize + QLatin1Char(' ') + tr("bytes") + QLatin1Char(')'));
        insertNewLineIntoDetails();
    }
}

void Update::downloadFile(const QString& fileName)
{
    insertInfoIntoDetails(fileName + QStringLiteral("... "));
    if (currentTriesCount_ == 0)
        ui_.currentFile->setText(
            QString::number(ui_.currentFile->text().toInt() + 1));

    const QNetworkReply* reply{
        downloadManager_.get(Networking::getDownloadFileRequest(fileName))};
    LOG(LogTypes::NETWORK,
        QStringLiteral("Sent request for downloading file ") +
            QString(fileName));
    ui_.progressBar->reset();
    connect(reply, &QNetworkReply::downloadProgress, this,
            &Update::updateProgress);
}

void Update::downloadFinished(QNetworkReply* reply)
{
    reply->deleteLater();
    if (!isReplyOk(reply))
        return;

    insertSuccessInfoIntoDetails(tr("Downloaded"));
    insertInfoIntoDetails(QStringLiteral("... "));

    QString fileName(filesToDownload_.takeFirst());
    QString fileSize(filesToDownloadSize_.takeFirst());

    QByteArray fileDownloadedContent{reply->readAll()};

    LOG(LogTypes::NETWORK, QStringLiteral("Expected file size ") +
                               QString::number(fileSize.toInt()) +
                               QStringLiteral(", size of downloaded file ") +
                               QString::number(fileDownloadedContent.size()) +
                               QLatin1Char('.'));

    // Verification of file size.
    if (fileDownloadedContent.size() == fileSize.toInt())
        saveVerfiedFile(fileDownloadedContent, fileName);
    else
    {
        // Do not continue if max tries reached.
        if (!handleVerificationError(fileName, fileSize))
            return;
    }

    if (filesToDownload_.isEmpty())
        finalizeUpdate();

    downloadFile(filesToDownload_.constFirst());
}

void Update::saveVerfiedFile(const QByteArray& fileData,
                             const QString& fileName)
{
    insertSuccessInfoIntoDetails(tr("Verified"));
    insertNewLineIntoDetails();

    QFile file(QApplication::applicationDirPath() + QLatin1Char('/') +
               fileName + tmpPrefix_);
    file.remove();
    file.open(QIODevice::WriteOnly);
    file.write(fileData);

    LOG(LogTypes::NETWORK,
        QStringLiteral("Saved file: ") + fileName + tmpPrefix_);

    tempFiles_.push_back(file.fileName());
    currentTriesCount_ = 0;
}

bool Update::handleVerificationError(const QString& fileName,
                                     const QString& fileSize)
{
    insertErrorInfoIntoDetails(tr("Error during verification!"));

    // Add corrupted file to front of vector if max tries not reached yet.
    if (currentTriesCount_ >= Networking::getMaxRetries())
    {
        insertErrorInfoIntoDetails(tr("Can not download file."));
        const QString msg(tr(
            "Updating interrupted - downloading error. Please retry later."));
        QMessageBox::critical(this, tr("Updating interrupted"), msg);
        return false;
    }

    filesToDownload_.push_front(fileName);
    filesToDownloadSize_.push_front(fileSize);
    ++currentTriesCount_;

    return true;
}

void Update::renameTempFile(const QString& file)
{
    QString targetFileName(file);
    targetFileName.chop(tmpPrefix_.size());

    insertInfoIntoDetails(file.section(QLatin1Char('/'), -1) +
                          QStringLiteral(" -> ") +
                          targetFileName.section(QLatin1Char('/'), -1));
    insertNewLineIntoDetails();

    QFile::remove(targetFileName);
    QFile::rename(file, targetFileName);
}

void Update::finalizeUpdate()
{
    insertNewLineIntoDetails();
    insertNewSectionIntoDetails(tr("Renaming temporary filenames") +
                                QLatin1Char(':'));

    for (const QString& tempFileName : qAsConst(tempFiles_))
        renameTempFile(tempFileName);

    insertNewLineIntoDetails();
    insertSuccessInfoIntoDetails(tr("Update complete") + QLatin1Char('.'));
    insertNewLineIntoDetails();

    ui_.valueActual->setText(ui_.valueAvailable->text());

    QMessageBox::information(nullptr, tr("Update complete"),
                             tr("Application update is completed."));

    QCoreApplication::quit();
}

void Update::updateProgress(qint64 bytesRead, qint64 totalBytes)
{
    ui_.progressBar->setMaximum(static_cast<int>(totalBytes));
    ui_.progressBar->setValue(static_cast<int>(bytesRead));
}

void Update::closeEvent(QCloseEvent* event)
{
    QWidget::closeEvent(event);

    // If logger window is shown closing mainWindow do not close app.
    QApplication::closeAllWindows();
}

void Update::showErrorMsg(const QString& error)
{
    if (!ui_.details->isVisible())
        ui_.showDetails->setChecked(true);

    // Do not close app. Allow user to check details.
    QMessageBox::critical(this, tr("Error"), error);
}

void Update::insertNewSectionIntoDetails(const QString& msg)
{
    ui_.details->insertHtml(QStringLiteral("<b><FONT COLOR=blue>") + msg +
                            QStringLiteral("</FONT></b><br>"));
    ui_.details->ensureCursorVisible();
}

void Update::insertInfoIntoDetails(const QString& msg)
{
    ui_.details->insertHtml(QStringLiteral("<FONT COLOR=black>") + msg +
                            QStringLiteral("</FONT>"));
    ui_.details->ensureCursorVisible();
}

void Update::insertNewLineIntoDetails()
{
    ui_.details->insertHtml(QStringLiteral("<br>"));
    ui_.details->ensureCursorVisible();
}

void Update::insertSuccessInfoIntoDetails(const QString& msg)
{
    ui_.details->insertHtml(QStringLiteral("<b><FONT COLOR=green>") + msg +
                            QStringLiteral("</FONT></b>"));
    ui_.details->ensureCursorVisible();
}

void Update::insertErrorInfoIntoDetails(const QString& msg)
{
    ui_.details->insertHtml(QStringLiteral("<b><FONT COLOR=red>") + msg +
                            QStringLiteral("</FONT></b>"));
    ui_.details->ensureCursorVisible();
}

void Update::showDetailsToggled(bool checked)
{
    static int detailsSize{0};
    if (checked)
    {
        const int minimumSize{ui_.verticalLayout->minimumSize().height()};
        ui_.details->show();
        setMinimumHeight(minimumSize + ui_.details->minimumHeight());
        resize(width(), minimumSize + detailsSize);
    }
    else
    {
        detailsSize = ui_.details->height();
        ui_.details->hide();
        const int minimumSize{ui_.verticalLayout->minimumSize().height()};
        setMinimumHeight(minimumSize);
    }
}
