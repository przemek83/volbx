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

    MainTab& operator=(const MainTab& other) = delete;
    MainTab(const MainTab& other) = delete;

    MainTab& operator=(MainTab&& other) = delete;
    MainTab(MainTab&& other) = delete;

    FilteringProxyModel* getCurrentProxyModel();

    TableModel* getCurrentDataModel();

    DataView* getCurrentDataView();
};

#endif  // MAINTAB_H
