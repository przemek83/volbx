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
    auto visualization{findChild<DatasetVisualization*>()};
    auto columnsPreview{findChild<ColumnsPreview*>()};
    auto datasetsListBrowser{findChild<DatasetsListBrowser*>()};

    if (datasetsListBrowser == nullptr || visualization == nullptr ||
        columnsPreview == nullptr)
        return;

    if (current.isEmpty())
    {
        columnsPreview->clearDataAndDisable();
        datasetsListBrowser->clearSelection();
        visualization->clear();
        visualization->setEnabled(false);
        Q_EMIT definitionIsReady(false);
    }
    else
    {
        std::unique_ptr<Dataset> dataset{
            std::make_unique<DatasetInner>(current)};

        // If definition is valid, than fill details.
        if (dataset->initialize() && dataset->isValid())
        {
            columnsPreview->setDatasetSampleInfo(*dataset);
            columnsPreview->setEnabled(true);

            visualization->setDataset(std::move(dataset));
            visualization->setEnabled(true);

            Q_EMIT definitionIsReady(true);
        }
        else
        {
            columnsPreview->clearDataAndDisable();
            visualization->clear();
            visualization->setEnabled(false);
            Q_EMIT definitionIsReady(false);

            QMessageBox::information(
                this, tr("Damaged dataset"),
                tr("Dataset ") + current + tr(" is damaged."));
        }
    }
}

bool DatasetImportTab::datasetsAreAvailable()
{
    const auto datasetsListBrowser{findChild<DatasetsListBrowser*>()};
    if (datasetsListBrowser == nullptr)
        return false;

    return (!datasetsListBrowser->isDatasetsListEmpty());
}
