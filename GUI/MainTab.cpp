#include "MainTab.h"

#include "Datasets/Dataset.h"
#include "ModelsAndViews/DataView.h"
#include "ModelsAndViews/FilteringProxyModel.h"
#include "ModelsAndViews/TableModel.h"

#include "ViewDockWidget.h"

MainTab::MainTab(std::unique_ptr<Dataset> dataset, QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle(dataset->getName());

    auto model = new TableModel(std::move(dataset), this);

    setDockNestingEnabled(true);

    auto proxyModel = new FilteringProxyModel(this);
    proxyModel->setSourceModel(model);

    ViewDockWidget* dock = new ViewDockWidget(tr("Data"), this);
    auto view = new DataView(dock);
    view->setModel(proxyModel);
    dock->setWidget(view);
    addDockWidget(Qt::LeftDockWidgetArea, dock);
}

FilteringProxyModel* MainTab::getCurrentProxyModel()
{
    auto proxyModel = findChild<FilteringProxyModel*>();
    Q_ASSERT(nullptr != proxyModel);
    return proxyModel;
}

TableModel* MainTab::getCurrentDataModel()
{
    auto dataModel = findChild<TableModel*>();
    Q_ASSERT(nullptr != dataModel);
    return dataModel;
}

DataView* MainTab::getCurrentDataView()
{
    auto dataView = findChild<DataView*>();
    Q_ASSERT(nullptr != dataView);
    return dataView;
}
