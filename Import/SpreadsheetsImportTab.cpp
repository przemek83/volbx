#include "SpreadsheetsImportTab.h"

#include <cmath>
#include <future>

#include <ProgressBarInfinite.h>
#include <QFileDialog>
#include <QFileInfo>
#include <QHeaderView>
#include <QMessageBox>
#include <QSplitter>
#include <QVBoxLayout>

#include "Common/Configuration.h"
#include "Common/Constants.h"
#include "Datasets/DatasetDefinitionOds.h"
#include "Datasets/DatasetDefinitionSpreadsheet.h"
#include "Datasets/DatasetDefinitionXlsx.h"
#include "Shared/Logger.h"

#include "ColumnsPreview.h"
#include "DatasetDefinitionVisualization.h"
#include "DatasetsListBrowser.h"
#include "ui_SpreadsheetsImportTab.h"

SpreadsheetsImportTab::SpreadsheetsImportTab(QWidget* parent)
    : ImportTab(parent), ui(new Ui::SpreadsheetsImportTab)
{
    ui->setupUi(this);

    connect(ui->openFileButton, &QPushButton::clicked, this,
            &SpreadsheetsImportTab::openFileButtonClicked);

    auto visualization = new DatasetDefinitionVisualization(this);

    auto splitter2 = new QSplitter(Qt::Vertical, this);
    splitter2->addWidget(visualization);
    auto columnsPreview = new ColumnsPreview(this);
    splitter2->addWidget(columnsPreview);

    ui->verticalLayout->addWidget(splitter2);

    const int rowHeight = lround(fontMetrics().height() * 1.5);
    columnsPreview->verticalHeader()->setDefaultSectionSize(rowHeight);

    visualization->setEnabled(false);
    columnsPreview->setEnabled(false);

    connect(visualization,
            &DatasetDefinitionVisualization::currentColumnNeedSync,
            columnsPreview, &ColumnsPreview::selectCurrentColumn);

    connect(columnsPreview, &ColumnsPreview::currentColumnNeedSync,
            visualization,
            &DatasetDefinitionVisualization::selectCurrentColumn);

    ui->sheetCombo->hide();
}

SpreadsheetsImportTab::~SpreadsheetsImportTab() { delete ui; }

void SpreadsheetsImportTab::analyzeFile(
    std::unique_ptr<DatasetDefinitionSpreadsheet>& datasetDefinition)
{
    const QString barTitle =
        Constants::getProgressBarTitle(Constants::BarTitle::ANALYSING);
    ProgressBarInfinite bar(barTitle, nullptr);
    bar.showDetached();
    bar.start();
    QTime performanceTimer;
    performanceTimer.start();
    QApplication::processEvents();

    bool success{false};
    // TODO get rid of get() on smart pointer.
    auto futureInit = std::async(&DatasetDefinitionSpreadsheet::init,
                                 datasetDefinition.get());
    std::chrono::milliseconds span(1);
    while (futureInit.wait_for(span) == std::future_status::timeout)
        QCoreApplication::processEvents();
    success = futureInit.get();
    if (!success)
    {
        LOG(LogTypes::IMPORT_EXPORT, datasetDefinition->getError());
        return;
    }

    LOG(LogTypes::IMPORT_EXPORT,
        "Analysed file having " +
            QString::number(datasetDefinition->rowCount()) + " rows in time " +
            QString::number(performanceTimer.elapsed() * 1.0 / 1000) +
            " seconds.");
}

void SpreadsheetsImportTab::openFileButtonClicked()
{
    QString fileName = QFileDialog::getOpenFileName(
        this, tr("Open file"), Configuration::getInstance().getImportFilePath(),
        tr("Spreadsheets (*.xlsx *.ods )"));

    if (fileName.isEmpty())
    {
        return;
    }

    QFileInfo fileInfo(fileName);
    if (!fileInfo.exists() || !fileInfo.isReadable())
    {
        QMessageBox::information(this, tr("Access error"),
                                 tr("Can not access file."));
        return;
    }

    Configuration::getInstance().setImportFilePath(fileInfo.absolutePath());

    ui->fileNameLineEdit->setText(fileName);

    std::unique_ptr<DatasetDefinitionSpreadsheet> datasetDefinition{nullptr};

    // Remove all not allowed characters from file name.
    QString regexpString = Constants::getDatasetNameRegExp().replace(
        QLatin1String("["), QLatin1String("[^"));
    QString datasetName =
        fileInfo.completeBaseName().remove(QRegExp(regexpString));

    if (datasetName.isEmpty())
    {
        datasetName = tr("Dataset");
    }

    if (0 == fileInfo.suffix().toLower().compare(QLatin1String("ods")))
    {
        datasetDefinition =
            std::make_unique<DatasetDefinitionOds>(datasetName, fileName);
    }
    else
    {
        if (0 == fileInfo.suffix().toLower().compare(QLatin1String("xlsx")))
        {
            datasetDefinition =
                std::make_unique<DatasetDefinitionXlsx>(datasetName, fileName);
        }
        else
        {
            QMessageBox::information(this, tr("Wrong file"),
                                     tr("File type is not supported."));
            Q_EMIT definitionIsReady(false);
            return;
        }
    }

    analyzeFile(datasetDefinition);

    auto visualization = findChild<DatasetDefinitionVisualization*>();
    if (nullptr == visualization)
    {
        return;
    }

    auto columnsPreview = findChild<ColumnsPreview*>();
    if (nullptr == columnsPreview)
    {
        return;
    }

    columnsPreview->setDatasetDefinitionSampleInfo(*datasetDefinition);
    columnsPreview->setEnabled(true);

    visualization->setDatasetDefiniton(std::move(datasetDefinition));
    visualization->setEnabled(true);

    Q_EMIT definitionIsReady(true);
}

std::unique_ptr<DatasetDefinition> SpreadsheetsImportTab::getDatasetDefinition()
{
    auto definition = findChild<DatasetDefinitionVisualization*>();
    return definition->retrieveDatasetDefinition();
}
