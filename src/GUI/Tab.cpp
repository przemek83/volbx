#include "Tab.h"

#include <Datasets/Dataset.h>
#include <ModelsAndViews/DataView.h>

#include "DataViewDock.h"

Tab::Tab(std::unique_ptr<Dataset> dataset, QString name, QWidget* parent)
    : QMainWindow(parent), proxyModel_{this}, model_{std::move(dataset)}
{
    setWindowTitle(name);

    setDockNestingEnabled(true);

    model_.setParent(this);
    proxyModel_.setSourceModel(&model_);

    addDockWidget(Qt::LeftDockWidgetArea, createDataViewDock());
}

FilteringProxyModel* Tab::getCurrentProxyModel() const
{
    return findChild<FilteringProxyModel*>();
}

TableModel* Tab::getCurrentTableModel() const
{
    return findChild<TableModel*>();
}

DataView* Tab::getCurrentDataView() const { return findChild<DataView*>(); }

DataViewDock* Tab::createDataViewDock()
{
    auto* dock{new DataViewDock(tr("Data"), this)};
    auto* view{new DataView(dock)};
    view->setModel(&proxyModel_);
    dock->setWidget(view);
    return dock;
}
