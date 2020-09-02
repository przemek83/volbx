#include "DatasetImportTab.h"

#include <DatasetInner.h>
#include <QHeaderView>
#include <QMessageBox>
#include <QSplitter>
#include <QVBoxLayout>

#include "ColumnsPreview.h"
#include "DatasetVisualization.h"
#include "DatasetsListBrowser.h"

DatasetImportTab::DatasetImportTab(QWidget* parent) : ImportTab(parent)
{
    auto [visualization, columnsPreview] =
        createVisualizationAndColumnPreview();
    auto listBrowser{new DatasetsListBrowser(this)};

    auto upperSplitter{new QSplitter(Qt::Horizontal, this)};
    upperSplitter->addWidget(listBrowser);
    upperSplitter->addWidget(visualization);

    auto centralSplitter{new QSplitter(Qt::Vertical, this)};
    centralSplitter->addWidget(upperSplitter);
    centralSplitter->addWidget(columnsPreview);

    auto layout{new QVBoxLayout(this)};
    layout->setContentsMargins(2, 2, 2, 2);
    layout->addWidget(centralSplitter);
    setLayout(layout);

    connect(listBrowser, &DatasetsListBrowser::currentDatasetChanged, this,
            &DatasetImportTab::selectedDatasetChanged);
}

std::unique_ptr<Dataset> DatasetImportTab::getDataset()
{
    auto definition{findChild<DatasetVisualization*>()};
    return definition->retrieveDataset();
}

void DatasetImportTab::selectedDatasetChanged(const QString& current)
{
    if (current.isEmpty())
        clear();
    else
        createDataset(current);
}

bool DatasetImportTab::datasetsAreAvailable()
{
    const auto datasetsListBrowser{findChild<DatasetsListBrowser*>()};
    return (!datasetsListBrowser->isDatasetsListEmpty());
}

void DatasetImportTab::clear()
{
    auto columnsPreview{findChild<ColumnsPreview*>()};
    columnsPreview->clearDataAndDisable();
    auto listBrowser{findChild<DatasetsListBrowser*>()};
    listBrowser->clearSelection();
    auto visualization{findChild<DatasetVisualization*>()};
    visualization->clear();
    visualization->setEnabled(false);

    Q_EMIT datasetIsReady(false);
}

void DatasetImportTab::createDataset(const QString& datasetName)
{
    std::unique_ptr<Dataset> dataset{
        std::make_unique<DatasetInner>(datasetName)};

    if (dataset->initialize() && dataset->isValid())
    {
        auto columnsPreview{findChild<ColumnsPreview*>()};
        columnsPreview->setDatasetSampleInfo(*dataset);
        columnsPreview->setEnabled(true);
        auto visualization{findChild<DatasetVisualization*>()};
        visualization->setDataset(std::move(dataset));
        visualization->setEnabled(true);

        Q_EMIT datasetIsReady(true);
    }
    else
    {
        clear();
        QMessageBox::information(
            this, tr("Damaged dataset"),
            tr("Dataset ") + datasetName + tr(" is damaged."));
    }
}
