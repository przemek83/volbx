#include "SpreadsheetsImportTab.h"

#include <cmath>
#include <future>

#include <ProgressBarInfinite.h>
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
#include "ui_SpreadsheetsImportTab.h"

SpreadsheetsImportTab::SpreadsheetsImportTab(QWidget* parent)
    : ImportTab(parent), ui(new Ui::SpreadsheetsImportTab)
{
    ui->setupUi(this);

    connect(ui->openFileButton, &QPushButton::clicked, this,
            &SpreadsheetsImportTab::openFileButtonClicked);

    auto visualization{new DatasetVisualization(this)};

    auto splitter2{new QSplitter(Qt::Vertical, this)};
    splitter2->addWidget(visualization);
    auto columnsPreview{new ColumnsPreview(this)};
    splitter2->addWidget(columnsPreview);

    ui->verticalLayout->addWidget(splitter2);

    const long rowHeight{lround(fontMetrics().height() * 1.5)};
    columnsPreview->verticalHeader()->setDefaultSectionSize(rowHeight);

    visualization->setEnabled(false);
    columnsPreview->setEnabled(false);

    connect(visualization, &DatasetVisualization::currentColumnNeedSync,
            columnsPreview, &ColumnsPreview::selectCurrentColumn);

    connect(columnsPreview, &ColumnsPreview::currentColumnNeedSync,
            visualization, &DatasetVisualization::selectCurrentColumn);

    ui->sheetCombo->hide();
}

SpreadsheetsImportTab::~SpreadsheetsImportTab() { delete ui; }

void SpreadsheetsImportTab::analyzeFile(
    std::unique_ptr<DatasetSpreadsheet>& dataset)
{
    const QString barTitle{
        Constants::getProgressBarTitle(Constants::BarTitle::ANALYSING)};
    ProgressBarInfinite bar(barTitle, nullptr);
    bar.showDetached();
    bar.start();
    QTime performanceTimer;
    performanceTimer.start();
    QApplication::processEvents();

    bool success{false};
    // TODO get rid of get() on smart pointer.
    auto futureInit{std::async(&Dataset::initialize, dataset.get())};
    const std::chrono::milliseconds span(1);
    while (futureInit.wait_for(span) == std::future_status::timeout)
        QCoreApplication::processEvents();
    success = futureInit.get();
    if (!success)
    {
        LOG(LogTypes::IMPORT_EXPORT, dataset->getLastError());
        return;
    }

    LOG(LogTypes::IMPORT_EXPORT,
        "Analysed file having " + QString::number(dataset->rowCount()) +
            " rows in time " +
            QString::number(performanceTimer.elapsed() * 1.0 / 1000) +
            " seconds.");
}

void SpreadsheetsImportTab::openFileButtonClicked()
{
    QString fileName = QFileDialog::getOpenFileName(
        this, tr("Open file"), Configuration::getInstance().getImportFilePath(),
        tr("Spreadsheets (*.xlsx *.ods )"));

    if (fileName.isEmpty())
        return;

    QFileInfo fileInfo(fileName);
    if (!fileInfo.exists() || !fileInfo.isReadable())
    {
        QMessageBox::information(this, tr("Access error"),
                                 tr("Can not access file."));
        return;
    }

    Configuration::getInstance().setImportFilePath(fileInfo.absolutePath());

    ui->fileNameLineEdit->setText(fileName);

    std::unique_ptr<DatasetSpreadsheet> dataset{nullptr};

    // Remove all not allowed characters from file name.
    QString regexpString{DatasetUtilities::getDatasetNameRegExp().replace(
        QLatin1String("["), QLatin1String("[^"))};
    QString datasetName{
        fileInfo.completeBaseName().remove(QRegExp(regexpString))};

    if (datasetName.isEmpty())
        datasetName = tr("Dataset");

    if (fileInfo.suffix().toLower().compare(QLatin1String("ods")) == 0)
        dataset = std::make_unique<DatasetOds>(datasetName, fileName);
    else
    {
        if (fileInfo.suffix().toLower().compare(QLatin1String("xlsx")) == 0)
            dataset = std::make_unique<DatasetXlsx>(datasetName, fileName);
        else
        {
            QMessageBox::information(this, tr("Wrong file"),
                                     tr("File type is not supported."));
            Q_EMIT definitionIsReady(false);
            return;
        }
    }

    analyzeFile(dataset);

    auto visualization{findChild<DatasetVisualization*>()};
    if (visualization == nullptr)
        return;

    auto columnsPreview{findChild<ColumnsPreview*>()};
    if (columnsPreview == nullptr)
        return;

    columnsPreview->setDatasetSampleInfo(*dataset);
    columnsPreview->setEnabled(true);

    visualization->setDataset(std::move(dataset));
    visualization->setEnabled(true);

    Q_EMIT definitionIsReady(true);
}

std::unique_ptr<Dataset> SpreadsheetsImportTab::getDataset()
{
    auto definition{findChild<DatasetVisualization*>()};
    return definition->retrieveDataset();
}
