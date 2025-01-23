#include "Tab.h"

#include <Datasets/Dataset.h>

Tab::Tab(std::unique_ptr<Dataset> dataset, QString name, QWidget* parent)
    : QMainWindow(parent),
      proxyModel_{this},
      model_{std::move(dataset)},
      dock_{tr("Data"), this},
      view_{&dock_}
{
    setWindowTitle(name);

    setDockNestingEnabled(true);

    model_.setParent(this);
    proxyModel_.setSourceModel(&model_);

    setupDock();

    addDockWidget(Qt::LeftDockWidgetArea, &dock_);
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

void Tab::setupDock()
{
    view_.setModel(&proxyModel_);
    dock_.setWidget(&view_);
}
