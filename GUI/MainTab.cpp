#include "Datasets/Dataset.h"
#include "ModelsAndViews/TableModel.h"
#include "ModelsAndViews/DataView.h"
#include "ModelsAndViews/FilteringProxyModel.h"

#include "MainTab.h"
#include "ViewDockWidget.h"

MainTab::MainTab(Dataset* dataset, QWidget *parent) :
    QMainWindow(parent)
{
    setWindowTitle(dataset->getName());

    TableModel* model = new TableModel(dataset, this);

    setDockNestingEnabled(true);

    FilteringProxyModel* proxyModel = new FilteringProxyModel(this);
    proxyModel->setSourceModel(model);

    ViewDockWidget* dock = new ViewDockWidget(tr("Data"), this);
    DataView* view = new DataView(dock);
    view->setModel(proxyModel);
    dock->setWidget(view);
    addDockWidget(Qt::LeftDockWidgetArea, dock);
}

MainTab::~MainTab()
{

}

FilteringProxyModel* MainTab::getCurrentProxyModel()
{
    FilteringProxyModel* proxyModel = findChild<FilteringProxyModel*>();
    Q_ASSERT(nullptr != proxyModel);
    return proxyModel;
}

TableModel* MainTab::getCurrentDataModel()
{
    TableModel* dataModel = findChild<TableModel*>();
    Q_ASSERT(nullptr != dataModel);
    return dataModel;
}

DataView* MainTab::getCurrentDataView()
{
    DataView* dataView = findChild<DataView*>();
    Q_ASSERT(nullptr != dataView);
    return dataView;
}
