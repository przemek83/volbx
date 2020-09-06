#include "MainTab.h"

#include <Datasets/Dataset.h>
#include <ModelsAndViews/DataView.h>
#include <ModelsAndViews/FilteringProxyModel.h>
#include <ModelsAndViews/TableModel.h>

#include "DataViewDock.h"

MainTab::MainTab(std::unique_ptr<Dataset> dataset, QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle(dataset->getName());

    setDockNestingEnabled(true);

    auto proxyModel{new FilteringProxyModel(this)};
    auto model{new TableModel(std::move(dataset), this)};
    proxyModel->setSourceModel(model);

    addDockWidget(Qt::LeftDockWidgetArea, createDataViewDock(proxyModel));
}

FilteringProxyModel* MainTab::getCurrentProxyModel()
{
    return findChild<FilteringProxyModel*>();
}

TableModel* MainTab::getCurrentTableModel() { return findChild<TableModel*>(); }

DataView* MainTab::getCurrentDataView() { return findChild<DataView*>(); }

DataViewDock* MainTab::createDataViewDock(FilteringProxyModel* proxyModel)
{
    auto dock{new DataViewDock(tr("Data"), this)};
    auto view{new DataView(dock)};
    view->setModel(proxyModel);
    dock->setWidget(view);
    return dock;
}
