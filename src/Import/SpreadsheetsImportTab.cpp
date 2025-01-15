#include "SpreadsheetsImportTab.h"

#include <cmath>
#include <future>

#include <wble/ProgressBarInfinite.h>
#include <QElapsedTimer>
#include <QFileDialog>
#include <QFileInfo>
#include <QHeaderView>
#include <QMessageBox>
#include <QSplitter>

#include <Common/Configuration.h>
#include <Common/Constants.h>
#include <Common/DatasetUtilities.h>
#include <Datasets/Dataset.h>
#include <Datasets/DatasetOds.h>
#include <Datasets/DatasetSpreadsheet.h>
#include <Datasets/DatasetXlsx.h>
#include <Shared/Logger.h>

#include "ColumnsPreview.h"
#include "DatasetVisualization.h"

SpreadsheetsImportTab::SpreadsheetsImportTab(QWidget* parent)
    : ImportTab(parent), ui_(std::make_unique<Ui::SpreadsheetsImportTab>())
{
    ui_->setupUi(this);

    connect(ui_->openFileButton, &QPushButton::clicked, this,
            &SpreadsheetsImportTab::openFileButtonClicked);

    auto [visualization, columnsPreview] =
        createVisualizationAndColumnPreview();

    auto* centralSplitter{new QSplitter(Qt::Vertical, this)};
    centralSplitter->addWidget(visualization);
    centralSplitter->addWidget(columnsPreview);

    ui_->verticalLayout->addWidget(centralSplitter);

    ui_->sheetCombo->hide();
}

void SpreadsheetsImportTab::analyzeFile(const std::unique_ptr<Dataset>& dataset)
{
    const QString barTitle{
        constants::getProgressBarTitle(constants::BarTitle::ANALYSING)};
    ProgressBarInfinite bar(barTitle, nullptr);
    bar.showDetached();
    bar.start();
    QElapsedTimer performanceTimer;
    performanceTimer.start();

    QCoreApplication::processEvents();

    auto futureInit{std::async(&Dataset::initialize, dataset.get())};
    const std::chrono::milliseconds span(1);
    while (futureInit.wait_for(span) == std::future_status::timeout)
        QCoreApplication::processEvents();
    if (!futureInit.get())
    {
        LOG(LogTypes::IMPORT_EXPORT, "Last error: " + dataset->getLastError());
        return;
    }

    LOG(LogTypes::IMPORT_EXPORT,
        "Analysed file having " + QString::number(dataset->rowCount()) +
            " rows in time " +
            constants::timeFromTimeToSeconds(performanceTimer) + " seconds.");
}

std::unique_ptr<Dataset> SpreadsheetsImportTab::createDataset(
    const QFileInfo& fileInfo)
{
    const QString datasetName{getValidDatasetName(fileInfo)};
    const QString datasetFilePath{fileInfo.canonicalFilePath()};

    std::unique_ptr<DatasetSpreadsheet> dataset{nullptr};
    if (fileInfo.suffix().toLower().compare(QStringLiteral("ods")) == 0)
        dataset = std::make_unique<DatasetOds>(datasetName, datasetFilePath);

    if (fileInfo.suffix().toLower().compare(QStringLiteral("xlsx")) == 0)
        dataset = std::make_unique<DatasetXlsx>(datasetName, datasetFilePath);

    return dataset;
}

bool SpreadsheetsImportTab::fileIsOk(const QFileInfo& fileInfo)
{
    return fileInfo.exists() && fileInfo.isReadable();
}

QString SpreadsheetsImportTab::getValidDatasetName(const QFileInfo& fileInfo)
{
    const QString regexpString{DatasetUtilities::getDatasetNameRegExp().replace(
        QStringLiteral("["), QStringLiteral("[^"))};
    QString datasetName{
        fileInfo.completeBaseName().remove(QRegularExpression(regexpString))};

    if (datasetName.isEmpty())
        datasetName = tr("Dataset");

    return datasetName;
}

bool SpreadsheetsImportTab::getFileInfo(QFileInfo& fileInfo)
{
    const QString defaultPath{Configuration::getInstance().getImportFilePath()};
    const QString filePath{
        QFileDialog::getOpenFileName(this, tr("Open file"), defaultPath,
                                     tr("Spreadsheets (*.xlsx *.ods )"))};

    fileInfo.setFile(filePath);
    if (!fileIsOk(fileInfo))
    {
        QMessageBox::information(this, tr("Access error"),
                                 tr("Can not access file."));
        return false;
    }
    return true;
}

void SpreadsheetsImportTab::openFileButtonClicked()
{
    QFileInfo fileInfo;
    if (!getFileInfo(fileInfo))
        return;

    Configuration::getInstance().setImportFilePath(fileInfo.canonicalPath());
    ui_->fileNameLineEdit->setText(fileInfo.filePath());

    std::unique_ptr<Dataset> dataset{createDataset(fileInfo)};
    if (dataset == nullptr)
    {
        QMessageBox::information(this, tr("Wrong file"),
                                 tr("File type is not supported."));
        Q_EMIT datasetIsReady(false);
        return;
    }

    analyzeFile(dataset);
    setDataset(std::move(dataset));
}
