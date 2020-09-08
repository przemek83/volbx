#include "Update.h"

#include <QFile>
#include <QMessageBox>
#include <QNetworkReply>

#include <Shared/Logger.h>
#include <Shared/Networking.h>

#include "ui_Update.h"

Update::Update(QWidget* parent) : QWidget(parent), ui(new Ui::Update)
{
    ui->setupUi(this);

    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint |
                   Qt::WindowCloseButtonHint);

    ui->valueAvailable->setText(tr("checking..."));
    ui->valueActual->setText(QApplication::applicationVersion());

    connect(&initialInfoNetworkManager_, &QNetworkAccessManager::finished, this,
            &Update::initialInfoNetworkReplyFinished);

    connect(&downloadManager_, &QNetworkAccessManager::finished, this,
            &Update::downloadFinished);

    // Get available version and files to update list.
    initialInfoNetworkManager_.get(Networking::getCurrentVersionRequest());

    insertInfoIntoDetails(tr("Connecting to update server") +
                          QStringLiteral("... "));

    LOG(LogTypes::NETWORK, QLatin1String("Initial network request send."));

    ui->details->hide();

    connect(ui->buttonQuit, &QPushButton::clicked, this,
            &Update::buttonQuitClicked);
    connect(ui->showDetails, &QCheckBox::toggled, this,
            &Update::showDetailsToggled);
}

Update::~Update() { delete ui; }

void Update::initialInfoNetworkReplyFinished(QNetworkReply* reply)
{
    reply->deleteLater();

    // Check errors.
    if (!Networking::replyIsValid(reply))
    {
        insertNewLineIntoDetails();
        insertErrorInfoIntoDetails(tr("Error") + QLatin1Char(':') +
                                   reply->errorString());
        showErrorMsg(tr("Connection error encountered."));
        return;
    }

    insertSuccessInfoIntoDetails(tr("Connected"));
    insertNewLineIntoDetails();
    insertNewLineIntoDetails();

    auto [newestVersion, replyStringList] =
        Networking::getAvailableVersionAndFiles(reply);

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

        downloadFile(filesToDownload_.constFirst());
    }
    else
    {
        QMessageBox::information(this, tr("No update needed"),
                                 tr("Application is up to date."));
        close();
    }
}

void Update::fillFilesToUpdateLists(QStringList& serverInfoList)
{
    // First line/element is correctness checker, second version.
    const int filesCount{serverInfoList.size() - 2};

    insertNewSectionIntoDetails(
        tr("Found") + QLatin1Char(' ') + QString::number(filesCount) +
        QLatin1Char(' ') + (filesCount != 1 ? tr("files") : tr("file")) +
        QLatin1Char(' ') + tr("to update") + QLatin1Char(':'));

    filesToDownload_.clear();
    filesToDownloadSize_.clear();
    for (int i = 0; i < filesCount; ++i)
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

    // Update number of downloaded file only when previous was downloaded
    // correctly.
    if (0 == currentTriesCount_)
    {
        ui->currentFile->setText(
            QString::number(ui->currentFile->text().toInt() + 1));
    }

    QNetworkReply* reply{
        downloadManager_.get(Networking::getDownloadFileRequest(fileName))};

    LOG(LogTypes::NETWORK, QLatin1String("Sent request for downloading file ") +
                               QString(fileName));

    ui->progressBar->reset();

    connect(reply, &QNetworkReply::downloadProgress, this,
            &Update::updateProgress);
}

void Update::downloadFinished(QNetworkReply* reply)
{
    reply->deleteLater();

    if (!Networking::replyIsValid(reply))
    {
        insertNewLineIntoDetails();
        insertErrorInfoIntoDetails(tr("Error") + QLatin1Char(':') +
                                   reply->errorString());
        showErrorMsg(tr("Connection error encountered."));
        return;
    }

    insertSuccessInfoIntoDetails(tr("Downloaded"));
    insertInfoIntoDetails(QStringLiteral("... "));

    QString fileName(filesToDownload_.takeFirst());
    QString fileSize(filesToDownloadSize_.takeFirst());

    QByteArray fileDownloadedContent{reply->readAll()};

    LOG(LogTypes::NETWORK, QLatin1String("Expected file size ") +
                               QString::number(fileSize.toInt()) +
                               QLatin1String(", size of downloaded file ") +
                               QString::number(fileDownloadedContent.size()) +
                               QLatin1Char('.'));

    // Verification of file size.
    if (fileDownloadedContent.size() == fileSize.toInt())
    {
        saveVerfiedFile(fileDownloadedContent, fileName);
    }
    else
    {
        // Do not continue if max tries reached.
        if (!handleVerificationError(fileName, fileSize))
            return;
    }

    if (filesToDownload_.isEmpty())
    {
        finalizeUpdate();

        close();

        return;
    }

    downloadFile(filesToDownload_.constFirst());
}

void Update::saveVerfiedFile(QByteArray& fileData, QString& fileName)
{
    QFile file(QApplication::applicationDirPath() + QLatin1Char('/') +
               fileName + tmpPrefix_);
    insertSuccessInfoIntoDetails(tr("Verified"));
    insertNewLineIntoDetails();
    file.remove();
    file.open(QIODevice::WriteOnly);
    file.write(fileData);

    LOG(LogTypes::NETWORK,
        QLatin1String("Saved file: ") + fileName + tmpPrefix_);

    file.close();
    tempFiles_.push_back(file.fileName());
    currentTriesCount_ = 0;
}

bool Update::handleVerificationError(QString& fileName, QString& fileSize)
{
    insertErrorInfoIntoDetails(tr("Error during verification!"));

    // Add corrupted file to front of vector if max tries not reached yet.
    if (currentTriesCount_ >= Networking::getMaxRetries())
    {
        insertErrorInfoIntoDetails(tr("Can not download file."));
        QString msg(tr(
            "Updating interrupted - downloading error. Please retry later."));
        QMessageBox::critical(this, tr("Updating interrupted"), msg);
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
    insertNewSectionIntoDetails(tr("Renaming temporary filenames") +
                                QLatin1Char(':'));
    for (const QString& tempFileName : tempFiles_)
    {
        QString targetFileName(tempFileName);
        targetFileName.chop(tmpPrefix_.size());

        insertInfoIntoDetails(tempFileName.section(QLatin1Char('/'), -1) +
                              QStringLiteral(" -> ") +
                              targetFileName.section(QLatin1Char('/'), -1));
        insertNewLineIntoDetails();

        QFile::remove(targetFileName);
        QFile::rename(tempFileName, targetFileName);
    }

    insertNewLineIntoDetails();
    insertSuccessInfoIntoDetails(tr("Update complete") + QLatin1Char('.'));
    insertNewLineIntoDetails();

    ui->valueActual->setText(ui->valueAvailable->text());

    QMessageBox::information(nullptr, tr("Update complete"),
                             tr("Application update is completed."));
}

void Update::updateProgress(qint64 bytesRead, qint64 totalBytes)
{
    ui->progressBar->setMaximum(static_cast<int>(totalBytes));
    ui->progressBar->setValue(static_cast<int>(bytesRead));
}

void Update::closeEvent(QCloseEvent* event)
{
    QWidget::closeEvent(event);

    // If logger window is shown closing mainWindow do not close app.
    QApplication::closeAllWindows();
}

void Update::buttonQuitClicked() { close(); }

void Update::showErrorMsg(const QString& error)
{
    if (!ui->details->isVisible())
        ui->showDetails->setChecked(true);

    // Do not close app. Allow user to check details.
    QMessageBox::critical(this, tr("Error"), error);
}

void Update::insertNewSectionIntoDetails(const QString& msg)
{
    ui->details->insertHtml(QStringLiteral("<b><FONT COLOR=blue>") + msg +
                            QStringLiteral("</FONT></b><br>"));
    ui->details->ensureCursorVisible();
}

void Update::insertInfoIntoDetails(const QString& msg)
{
    ui->details->insertHtml(QStringLiteral("<FONT COLOR=black>") + msg +
                            QStringLiteral("</FONT>"));
    ui->details->ensureCursorVisible();
}

void Update::insertNewLineIntoDetails()
{
    ui->details->insertHtml(QStringLiteral("<br>"));
    ui->details->ensureCursorVisible();
}

void Update::insertSuccessInfoIntoDetails(const QString& msg)
{
    ui->details->insertHtml(QStringLiteral("<b><FONT COLOR=green>") + msg +
                            QStringLiteral("</FONT></b>"));
    ui->details->ensureCursorVisible();
}

void Update::insertErrorInfoIntoDetails(const QString& msg)
{
    ui->details->insertHtml(QStringLiteral("<b><FONT COLOR=red>") + msg +
                            QStringLiteral("</FONT></b>"));
    ui->details->ensureCursorVisible();
}

void Update::showDetailsToggled(bool checked)
{
    static int detailsSize{0};
    if (checked)
    {
        int minimumSize{ui->verticalLayout->minimumSize().height()};
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
