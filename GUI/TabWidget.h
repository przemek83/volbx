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
 * @brief Central widget with tabs. Each tab contains data and plots.
 */
class TabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit TabWidget(QWidget* parent = nullptr);

    FilteringProxyModel* getCurrentProxyModel() const;

    TableModel* getCurrentDataModel() const;

    DataView* getCurrentDataView() const;

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
    bool plotExist() const;

    void changingFilterPreActions() const;

    void changingFilterPostActions() const;

    void activateDataSelection(DataView* view);

    DataViewDock* getCurrentDataViewDock() const;

    Tab* getCurrentMainTab() const;

    static QVector<std::pair<QString, int>> getStringColumnsWithIndexes(
        TableModel* model);

    template <class T>
    void showPlot();
};
