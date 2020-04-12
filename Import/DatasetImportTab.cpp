#include "DatasetImportTab.h"

#include <QHeaderView>
#include <QMessageBox>
#include <QSplitter>
#include <QVBoxLayout>

#include "Datasets/DatasetDefinitionInner.h"

#include "ColumnsPreview.h"
#include "DatasetDefinitionVisualization.h"
#include "DatasetsListBrowser.h"

DatasetImportTab::DatasetImportTab(QWidget* parent) : ImportTab(parent)
{
    // Create vertical and horizontal splitters and insert widgets into it.
    auto datasetsListBrowser = new DatasetsListBrowser(this);
    auto visualization = new DatasetDefinitionVisualization(this);

    auto splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(datasetsListBrowser);
    splitter->addWidget(visualization);

    auto splitter2 = new QSplitter(Qt::Vertical, this);
    splitter2->addWidget(splitter);
    auto columnsPreview = new ColumnsPreview(this);
    splitter2->addWidget(columnsPreview);

    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(2, 2, 2, 2);
    layout->addWidget(splitter2);
    setLayout(layout);

    const int rowHeight = static_cast<int>(fontMetrics().height() * 1.5);
    columnsPreview->verticalHeader()->setDefaultSectionSize(rowHeight);

    visualization->setEnabled(false);
    columnsPreview->setEnabled(false);

    connect(datasetsListBrowser, &DatasetsListBrowser::currentDatasetChanged,
            this, &DatasetImportTab::selectedDatasetChanged);

    connect(visualization,
            &DatasetDefinitionVisualization::currentColumnNeedSync,
            columnsPreview, &ColumnsPreview::selectCurrentColumn);

    connect(columnsPreview, &ColumnsPreview::currentColumnNeedSync,
            visualization,
            &DatasetDefinitionVisualization::selectCurrentColumn);
}

std::unique_ptr<DatasetDefinition> DatasetImportTab::getDatasetDefinition()
{
    auto definition = findChild<DatasetDefinitionVisualization*>();
    return definition->retrieveDatasetDefinition();
}

void DatasetImportTab::selectedDatasetChanged(const QString& current)
{
    auto visualization = findChild<DatasetDefinitionVisualization*>();
    auto columnsPreview = findChild<ColumnsPreview*>();
    auto datasetsListBrowser = findChild<DatasetsListBrowser*>();

    if (nullptr == datasetsListBrowser || nullptr == visualization ||
        nullptr == columnsPreview)
    {
        return;
    }

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
        std::unique_ptr<DatasetDefinition> datasetDefinition =
            std::make_unique<DatasetDefinitionInner>(current);

        // If definition is valid, than fill details.
        if (datasetDefinition && datasetDefinition->isValid())
        {
            columnsPreview->setDatasetDefinitionSampleInfo(*datasetDefinition);
            columnsPreview->setEnabled(true);

            visualization->setDatasetDefiniton(std::move(datasetDefinition));
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
    auto datasetsListBrowser = findChild<DatasetsListBrowser*>();
    if (nullptr == datasetsListBrowser)
    {
        return false;
    }

    return (!datasetsListBrowser->isDatasetsListEmpty());
}
