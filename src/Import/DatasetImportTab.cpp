#include "DatasetImportTab.h"

#include <DatasetInner.h>
#include <QHeaderView>
#include <QMessageBox>
#include <QSplitter>
#include <QVBoxLayout>

#include "ColumnsPreview.h"
#include "DatasetVisualization.h"
#include "DatasetsListBrowser.h"

DatasetImportTab::DatasetImportTab() : ImportTab()
{
    auto [visualization, columnsPreview]{createVisualizationAndPreview()};
    auto* listBrowser{new DatasetsListBrowser(this)};

    auto* upperSplitter{new QSplitter(Qt::Horizontal, this)};
    upperSplitter->addWidget(listBrowser);
    upperSplitter->addWidget(visualization);

    auto* centralSplitter{new QSplitter(Qt::Vertical, this)};
    centralSplitter->addWidget(upperSplitter);
    centralSplitter->addWidget(columnsPreview);

    auto* layout{new QVBoxLayout(this)};
    layout->setContentsMargins(2, 2, 2, 2);
    layout->addWidget(centralSplitter);
    setLayout(layout);

    connect(listBrowser, &DatasetsListBrowser::currentDatasetChanged, this,
            &DatasetImportTab::selectedDatasetChanged);
}

void DatasetImportTab::selectedDatasetChanged(const QString& current)
{
    if (current.isEmpty())
    {
        clear();
        auto* listBrowser{findChild<DatasetsListBrowser*>()};
        listBrowser->clearSelection();
    }
    else
        createDataset(current);
}

bool DatasetImportTab::datasetsAreAvailable() const
{
    const auto* datasetsListBrowser{findChild<DatasetsListBrowser*>()};
    return (!datasetsListBrowser->isDatasetsListEmpty());
}

void DatasetImportTab::clear()
{
    auto* columnsPreview{findChild<ColumnsPreview*>()};
    columnsPreview->clear();
    columnsPreview->setEnabled(false);
    auto* visualization{findChild<DatasetVisualization*>()};
    visualization->clear();
    visualization->setEnabled(false);

    emit datasetIsReady(false);
}

void DatasetImportTab::createDataset(const QString& datasetName)
{
    std::unique_ptr<Dataset> dataset{
        std::make_unique<DatasetInner>(datasetName)};

    if (dataset->initialize() && dataset->isValid())
    {
        setDataset(std::move(dataset));
    }
    else
    {
        clear();
        QMessageBox::information(
            this, tr("Damaged dataset"),
            tr("Dataset ") + datasetName + tr(" is damaged."));
    }
}
