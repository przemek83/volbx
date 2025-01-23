#pragma once

#include <memory>

#include <QMainWindow>

#include <ModelsAndViews/DataView.h>
#include <ModelsAndViews/FilteringProxyModel.h>
#include <ModelsAndViews/TableModel.h>

#include "DataViewDock.h"

class Dataset;

/// @brief Tab containing models, view, dock widgets with data and plot.
class Tab : public QMainWindow
{
    Q_OBJECT
public:
    Tab(std::unique_ptr<Dataset> dataset, const QString &name, QWidget* parent);

    FilteringProxyModel* getCurrentProxyModel() const;

    TableModel* getCurrentTableModel() const;

    DataView* getCurrentDataView() const;

private:
    void setupDock();

    FilteringProxyModel proxyModel_;
    TableModel model_;

    DataViewDock dock_;
    DataView view_;
};
