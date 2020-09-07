#pragma once

#include <functional>

#include <QDate>
#include <QTabWidget>

class TableModel;
class DataView;
class FilteringProxyModel;
class Tab;
class DataViewDock;
class PlotDock;

/**
 * @brief TabWidget. Each tab contains data and plots.
 */
class TabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit TabWidget(QWidget* parent = nullptr);

    ~TabWidget() override = default;

    FilteringProxyModel* getCurrentProxyModel();

    TableModel* getCurrentDataModel();

    DataView* getCurrentDataView();

public Q_SLOTS:
    void setTextFilter(int column, const QStringList& bannedStrings);

    void setDateFilter(int column, QDate from, QDate to, bool filterEmptyDates);

    void setNumericFilter(int column, double from, double to);

    void addBasicPlot();

    void addHistogramPlot();

    void addGroupingPlot();

private:
    template <class T>
    void addPlot(const QString& title, const std::function<T*()>& createPlot);

    template <class T>
    bool plotExist();

    void changingFilterPreActions();

    void changingFilterPostActions();

    void activateDataSelection(DataView* view);

    DataViewDock* getCurrentDataViewDock();

    Tab* getCurrentMainTab();

    QVector<std::pair<QString, int>> getStringColumnsWithIndexes(
        TableModel* model) const;

    template <class T>
    void showPlot();
};
