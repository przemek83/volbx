#pragma once

#include <QDate>
#include <QTabWidget>

#include "TabBar.h"

class TableModel;
class DataView;
class FilteringProxyModel;
class Tab;
class DataViewDock;
class PlotDock;

/// @brief Central widget with tabs. Each tab contains data and plots.
class TabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit TabWidget(QWidget* parent);

    FilteringProxyModel* getCurrentProxyModel() const;

    TableModel* getCurrentDataModel() const;

    DataView* getCurrentDataView() const;

public slots:
    void setTextFilter(int column, const QStringList& bannedStrings) const;

    void setDateFilter(int column, QDate from, QDate to,
                       bool filterEmptyDates) const;

    void setNumericFilter(int column, double from, double to) const;

    void addBasicPlot();

    void addHistogramPlot();

    void addGroupingPlot();

private:
    void addPlot(const QString& title, QWidget* plot) const;

    template <class T>
    bool plotExist() const;

    void changingFilterPreActions() const;

    void changingFilterPostActions() const;

    void activateDataSelection(DataView* view) const;

    DataViewDock* getCurrentDataViewDock() const;

    Tab* getCurrentMainTab() const;

    static QVector<std::pair<QString, int>> getStringColumnsWithIndexes(
        const TableModel* model);

    template <class T>
    void showPlot();

    TabBar tabBar_;
};
