#include "ImportTab.h"

#include <QHeaderView>

#include <Datasets/Dataset.h>

#include "ColumnsPreview.h"
#include "DatasetVisualization.h"

ImportTab::ImportTab(QWidget* parent) : QWidget(parent) {}

std::pair<DatasetVisualization*, ColumnsPreview*>
ImportTab::createVisualizationAndColumnPreview()
{
    auto* visualization{new DatasetVisualization(this)};
    auto* columnsPreview{new ColumnsPreview(this)};

    const int rowHeight{static_cast<int>(fontMetrics().height() * 1.5)};
    columnsPreview->verticalHeader()->setDefaultSectionSize(rowHeight);

    visualization->setEnabled(false);
    columnsPreview->setEnabled(false);

    connect(visualization, &DatasetVisualization::currentColumnNeedSync,
            columnsPreview, &ColumnsPreview::selectCurrentColumn);

    connect(columnsPreview, &ColumnsPreview::currentColumnNeedSync,
            visualization, &DatasetVisualization::selectCurrentColumn);

    return {visualization, columnsPreview};
}

std::unique_ptr<Dataset> ImportTab::getDataset()
{
    auto* definition{findChild<DatasetVisualization*>()};
    return definition->retrieveDataset();
}

void ImportTab::setDataset(std::unique_ptr<Dataset> dataset)
{
    auto* columnsPreview{findChild<ColumnsPreview*>()};
    columnsPreview->setDatasetSampleInfo(dataset);
    columnsPreview->setEnabled(true);
    auto* visualization{findChild<DatasetVisualization*>()};
    visualization->setDataset(std::move(dataset));
    visualization->setEnabled(true);

    Q_EMIT datasetIsReady(true);
}
