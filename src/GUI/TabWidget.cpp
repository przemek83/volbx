#include "TabWidget.h"

#include <qwtble/BasicDataPlot.h>
#include <qwtble/GroupPlotUI.h>
#include <qwtble/HistogramPlotUI.h>
#include <QApplication>

#include <Common/TimeLogger.h>
#include <ModelsAndViews/DataView.h>
#include <ModelsAndViews/FilteringProxyModel.h>
#include <ModelsAndViews/TableModel.h>

#include "DataViewDock.h"
#include "PlotDock.h"
#include "Tab.h"

TabWidget::TabWidget(QWidget* parent) : QTabWidget(parent)
{
    setTabBar(&tabBar_);
    setTabsClosable(true);
    setMovable(true);
}

FilteringProxyModel* TabWidget::getCurrentProxyModel() const
{
    return getCurrentMainTab()->getCurrentProxyModel();
}

TableModel* TabWidget::getCurrentDataModel() const
{
    return getCurrentMainTab()->getCurrentTableModel();
}

DataView* TabWidget::getCurrentDataView() const
{
    return getCurrentMainTab()->getCurrentDataView();
}

Tab* TabWidget::getCurrentMainTab() const
{
    auto* currentTab{dynamic_cast<Tab*>(currentWidget())};
    return currentTab;
}

QVector<std::pair<QString, int>> TabWidget::getStringColumnsWithIndexes(
    const TableModel* model)
{
    QVector<std::pair<QString, int>> stringColumns;
    const int count{model->columnCount()};
    for (int column{0}; column < count; ++column)
    {
        if (model->getColumnFormat(column) != ColumnType::STRING)
            continue;

        const QString columnName{
            model->headerData(column, Qt::Horizontal).toString()};
        stringColumns.append({columnName, column});
    }
    return stringColumns;
}

DataViewDock* TabWidget::getCurrentDataViewDock() const
{
    const DataView* dataView{getCurrentDataView()};
    return ::qobject_cast<DataViewDock*>(dataView->parent());
}

void TabWidget::changingFilterPreActions() const
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();
    getCurrentDataView()->clearSelection();
}

void TabWidget::changingFilterPostActions() const
{
    DataView* view{getCurrentDataView()};
    view->selectAll();
    view->recomputeAllData();
    QApplication::restoreOverrideCursor();
}

void TabWidget::setTextFilter(int column,
                              const QStringList& bannedStrings) const
{
    const TimeLogger timeLogger(LogTypes::CALC,
                                QStringLiteral("Filtration changed"));

    changingFilterPreActions();
    getCurrentProxyModel()->setStringFilter(column, bannedStrings);
    changingFilterPostActions();
}

void TabWidget::setDateFilter(int column, QDate from, QDate to,
                              bool filterEmptyDates) const
{
    const TimeLogger timeLogger(LogTypes::CALC,
                                QStringLiteral("Filtration changed"));

    changingFilterPreActions();
    getCurrentProxyModel()->setDateFilter(column, from, to, filterEmptyDates);
    changingFilterPostActions();
}

void TabWidget::setNumericFilter(int column, double from, double to) const
{
    const TimeLogger timeLogger(LogTypes::CALC,
                                QStringLiteral("Filtration changed"));

    changingFilterPreActions();
    getCurrentProxyModel()->setNumericFilter(column, from, to);
    changingFilterPostActions();
}

template <class T>
void TabWidget::showPlot()
{
    Tab* mainTab{getCurrentMainTab()};
    auto* plotUI{mainTab->findChild<T*>()};
    auto dock{::qobject_cast<PlotDock*>(plotUI->parent())};
    dock->setVisible(true);
    dock->raise();
}

template <class T>
bool TabWidget::plotExist() const
{
    Tab* mainTab{getCurrentMainTab()};
    const auto* plotUI{mainTab->findChild<T*>()};
    return plotUI != nullptr;
}

void TabWidget::addPlot(const QString& title, QWidget* plot) const
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();

    Tab* mainTab{getCurrentMainTab()};
    auto* tabifyOn{mainTab->findChild<PlotDock*>()};
    auto* dock{new PlotDock(title, mainTab)};
    dock->setWidget(plot);
    mainTab->addDockWidget(Qt::RightDockWidgetArea, dock);

    DataView* view{getCurrentDataView()};
    if (tabifyOn != nullptr)
        mainTab->tabifyDockWidget(tabifyOn, dock);
    else
        activateDataSelection(view);

    dock->setVisible(true);
    dock->raise();

    view->recomputeAllData();

    QApplication::restoreOverrideCursor();
}

void TabWidget::addBasicPlot()
{
    if (plotExist<BasicDataPlot>())
    {
        showPlot<BasicDataPlot>();
        return;
    }

    const DataView* view{getCurrentDataView()};
    auto* basicPlot{new BasicDataPlot()};
    connect(&(view->getPlotDataProvider()),
            &PlotDataProvider::basicPlotDataChanged, basicPlot,
            &BasicDataPlot::setNewData);

    addPlot(tr("Quantiles"), basicPlot);
}

void TabWidget::addHistogramPlot()
{
    if (plotExist<HistogramPlotUI>())
    {
        showPlot<HistogramPlotUI>();
        return;
    }

    const DataView* view{getCurrentDataView()};
    auto* histogramPlot{new HistogramPlotUI()};
    connect(&(view->getPlotDataProvider()),
            &PlotDataProvider::fundamentalDataChanged, histogramPlot,
            &HistogramPlotUI::setNewData);

    addPlot(tr("Histogram"), histogramPlot);
}

void TabWidget::addGroupingPlot()
{
    if (plotExist<GroupPlotUI>())
    {
        showPlot<GroupPlotUI>();
        return;
    }

    const DataView* view{getCurrentDataView()};
    const TableModel* model{getCurrentDataModel()};
    auto* groupPlot{new GroupPlotUI(getStringColumnsWithIndexes(model))};
    connect(&(view->getPlotDataProvider()),
            &PlotDataProvider::groupingPlotDataChanged, groupPlot,
            &GroupPlotUI::setNewData);
    connect(groupPlot, &GroupPlotUI::traitIndexChanged, view,
            &DataView::groupingColumnChanged);

    addPlot(tr("Grouping"), groupPlot);
}

void TabWidget::activateDataSelection(DataView* view) const
{
    const DataViewDock* viewDock{getCurrentDataViewDock()};
    viewDock->activateSelectButtons();

    view->clearSelection();
    view->setSelectionMode(QAbstractItemView::MultiSelection);
    view->selectAll();
}
