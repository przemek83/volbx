#pragma once

#include <functional>

#include <QDate>
#include <QTabWidget>

class TableModel;
class DataView;
class FilteringProxyModel;
class MainTab;
class DataViewDock;

/**
 * @brief TabWidget. Each tab contains data and plots.
 */
class MainTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit MainTabWidget(QWidget* parent = nullptr);

    ~MainTabWidget() override = default;

    FilteringProxyModel* getCurrentProxyModel();

    TableModel* getCurrentDataModel();

    DataView* getCurrentDataView();

public Q_SLOTS:
    void setTextFilterInProxy(int column, const QStringList& bannedStrings);

    void setDateFilterInProxy(int column, QDate from, QDate to,
                              bool filterEmptyDates);

    void setNumericFilterInProxy(int column, double from, double to);

    void addBasicPlot();

    void addHistogramPlot();

    void addGroupingPlot();

private:
    template <class T>
    void addPlot(const QString& title, const std::function<T*()>& createPlot);

    void activateDataSelection(DataView* view);

    DataViewDock* getCurrentDataViewDock();

    MainTab* getCurrentMainTab();

    QVector<std::pair<QString, int>> getStringColumnsWithIndexes(
        TableModel* model) const;
};
