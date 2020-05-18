#ifndef MAINTAB_H
#define MAINTAB_H

#include <memory>

#include <QMainWindow>

class Dataset;
class TableModel;
class DataView;
class FilteringProxyModel;

/**
 * @brief Main window added to main tabWidget.
 * It is parent for data dock, plot docks, related model and views.
 */
class MainTab : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainTab(std::unique_ptr<Dataset> dataset,
                     QWidget* parent = nullptr);

    ~MainTab() override = default;

    FilteringProxyModel* getCurrentProxyModel();

    TableModel* getCurrentDataModel();

    DataView* getCurrentDataView();
};

#endif  // MAINTAB_H
