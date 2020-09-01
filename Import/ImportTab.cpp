#include "ImportTab.h"

#include <QHeaderView>

#include "ColumnsPreview.h"
#include "DatasetVisualization.h"

ImportTab::ImportTab(QWidget* parent) : QWidget(parent) {}

std::pair<DatasetVisualization*, ColumnsPreview*>
ImportTab::createVisualizationAndColumnPreview()
{
    auto visualization{new DatasetVisualization(this)};
    auto columnsPreview{new ColumnsPreview(this)};

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
