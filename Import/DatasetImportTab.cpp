#include <QSplitter>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QMessageBox>

#include "DataSets/DatasetDefinitionInner.h"

#include "DatasetDefinitionVisualization.h"
#include "DatasetsListBrowser.h"
#include "ColumnsPreview.h"
#include "DatasetImportTab.h"

DatasetImportTab::DatasetImportTab(QWidget *parent) :
    ImportTab(parent)
{
    //Create vertical and horizontal splitters and insert widgets into it.
    DatasetsListBrowser* datasetsListBrowser = new DatasetsListBrowser(this);
    DatasetDefinitionVisualization* visualization =
        new DatasetDefinitionVisualization(this);

    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(datasetsListBrowser);
    splitter->addWidget(visualization);
    QList<int> sizes;
    sizes << 100 << 300;
    splitter->setSizes(sizes);

    QSplitter* splitter2 = new QSplitter(Qt::Vertical, this);
    splitter2->addWidget(splitter);
    ColumnsPreview* columnsPreview = new ColumnsPreview(this);
    splitter2->addWidget(columnsPreview);
    sizes.clear();
    sizes << 300 << 150;
    splitter2->setSizes(sizes);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(2, 2, 2, 2);
    layout->addWidget(splitter2);
    setLayout(layout);

    const int rowHeight = fontMetrics().height() * 1.5;
    columnsPreview->verticalHeader()->setDefaultSectionSize(rowHeight);

    visualization->setEnabled(false);
    columnsPreview->setEnabled(false);

    //Connect signals.
    connect(datasetsListBrowser,
            SIGNAL(currentDatasetChanged(QString)),
            this,
            SLOT(selectedDatasetChanged(QString)));

    connect(visualization,
            SIGNAL(currentColumnNeedSync(int)),
            columnsPreview,
            SLOT(selectCurrentColumn(int)));

    connect(columnsPreview,
            SIGNAL(currentColumnNeedSync(int)),
            visualization,
            SLOT(selectCurrentColumn(int)));
}

DatasetImportTab::~DatasetImportTab()
{

}

DatasetDefinition* DatasetImportTab::getDatasetDefinition()
{
    DatasetDefinitionVisualization* definition =
        findChild<DatasetDefinitionVisualization*>();
    return definition->getDatasetDefinition();
}

void DatasetImportTab::selectedDatasetChanged(QString current)
{
    DatasetDefinitionVisualization* visualization =
        findChild<DatasetDefinitionVisualization*>();

    ColumnsPreview* columnsPreview = findChild<ColumnsPreview*>();

    DatasetsListBrowser* datasetsListBrowser =
        findChild<DatasetsListBrowser*>();

    if( NULL == datasetsListBrowser ||
        NULL == visualization ||
        NULL == columnsPreview )
    {
        return;
    }

    if(true == current.isEmpty())
    {
        columnsPreview->clearDataAndDisable();
        datasetsListBrowser->clearSelection();
        visualization->clear();
        visualization->setEnabled(false);
        emit definitionIsReady(false);
    }
    else
    {
        DatasetDefinition* datasetDefinition =
            new DatasetDefinitionInner(current);

        //If definition is valid, than fill details.
        if ( NULL != datasetDefinition && datasetDefinition->isValid() )
        {
            visualization->setDatasetDefiniton(datasetDefinition);
            visualization->setEnabled(true);

            columnsPreview->setDatasetDefinitionSampleInfo(datasetDefinition);
            columnsPreview->setEnabled(true);

            emit definitionIsReady(true);
        }
        else
        {
            delete datasetDefinition;

            columnsPreview->clearDataAndDisable();
            visualization->clear();
            visualization->setEnabled(false);
            emit definitionIsReady(false);

            QMessageBox::information(this,
                                     tr("Damaged dataset"),
                                     tr("Dataset ") + current +
                                     tr(" is damaged."));
        }
    }
}

bool DatasetImportTab::datasetsAreAvailable()
{
    DatasetsListBrowser* datasetsListBrowser =
        findChild<DatasetsListBrowser*>();
    if( NULL == datasetsListBrowser )
    {
        return false;
    }

    return (false == datasetsListBrowser->isDatasetsListEmpty());
}
