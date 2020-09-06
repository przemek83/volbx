#pragma once

#include <memory>

#include <QMainWindow>

class Dataset;
class TableModel;
class DataView;
class FilteringProxyModel;
class DataViewDock;

/**
 * @brief Tab containing models, view, dock widgets with data and plot.
 */
class MainTab : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainTab(std::unique_ptr<Dataset> dataset,
                     QWidget* parent = nullptr);

    ~MainTab() override = default;

    FilteringProxyModel* getCurrentProxyModel();

    TableModel* getCurrentTableModel();

    DataView* getCurrentDataView();

private:
    DataViewDock* createDataViewDock(FilteringProxyModel* proxyModel);
};
