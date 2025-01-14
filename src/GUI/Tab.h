#pragma once

#include <memory>

#include <QMainWindow>

class Dataset;
class TableModel;
class DataView;
class FilteringProxyModel;
class DataViewDock;

/// @brief Tab containing models, view, dock widgets with data and plot.
class Tab : public QMainWindow
{
    Q_OBJECT
public:
    Tab(std::unique_ptr<Dataset> dataset, QWidget* parent);

    FilteringProxyModel* getCurrentProxyModel() const;

    TableModel* getCurrentTableModel() const;

    DataView* getCurrentDataView() const;

private:
    DataViewDock* createDataViewDock(FilteringProxyModel* proxyModel);
};
