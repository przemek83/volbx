#include "TabWidget.h"

#include <BasicDataPlot.h>
#include <GroupPlotUI.h>
#include <HistogramPlotUI.h>
#include <QApplication>

#include <Common/TimeLogger.h>
#include <ModelsAndViews/DataView.h>
#include <ModelsAndViews/FilteringProxyModel.h>
#include <ModelsAndViews/TableModel.h>

#include "DataViewDock.h"
#include "PlotDock.h"
#include "Tab.h"
#include "TabBar.h"

TabWidget::TabWidget(QWidget* parent) : QTabWidget(parent)
{
    setTabBar(new TabBar(this));
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
    TableModel* model)
{
    QVector<std::pair<QString, int>> stringColumns;
    for (int i = 0; i < model->columnCount(); ++i)
    {
        if (model->getColumnFormat(i) != ColumnType::STRING)
            continue;

        const QString columnName{
            model->headerData(i, Qt::Horizontal).toString()};
        stringColumns.append({columnName, i});
    }
    return stringColumns;
}

DataViewDock* TabWidget::getCurrentDataViewDock() const
{
    DataView* dataView{getCurrentDataView()};
    return qobject_cast<DataViewDock*>(dataView->parent());
}

void TabWidget::changingFilterPreActions()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();
    getCurrentDataView()->clearSelection();
}

void TabWidget::changingFilterPostActions()
{
    DataView* view{getCurrentDataView()};
    view->selectAll();
    view->recomputeAllData();
    QApplication::restoreOverrideCursor();
}

void TabWidget::setTextFilter(int column, const QStringList& bannedStrings)
{
    TimeLogger timeLogger(LogTypes::CALC, QStringLiteral("Filtration changed"));

    changingFilterPreActions();
    getCurrentProxyModel()->setStringFilter(column, bannedStrings);
    changingFilterPostActions();
}

void TabWidget::setDateFilter(int column, QDate from, QDate to,
                              bool filterEmptyDates)
{
    TimeLogger timeLogger(LogTypes::CALC, QStringLiteral("Filtration changed"));

    changingFilterPreActions();
    getCurrentProxyModel()->setDateFilter(column, from, to, filterEmptyDates);
    changingFilterPostActions();
}

void TabWidget::setNumericFilter(int column, double from, double to)
{
    TimeLogger timeLogger(LogTypes::CALC, QStringLiteral("Filtration changed"));

    changingFilterPreActions();
    getCurrentProxyModel()->setNumericFilter(column, from, to);
    changingFilterPostActions();
}

template <class T>
void TabWidget::showPlot()
{
    Tab* mainTab{getCurrentMainTab()};
    auto* plotUI{mainTab->findChild<T*>()};
    auto dock{qobject_cast<PlotDock*>(plotUI->parent())};
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

template <class T>
void TabWidget::addPlot(const QString& title,
                        const std::function<T*()>& createPlot)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();

    Tab* mainTab{getCurrentMainTab()};
    auto* tabifyOn{mainTab->findChild<PlotDock*>()};
    auto* dock{new PlotDock(title, mainTab)};
    dock->setWidget(createPlot());
    mainTab->addDockWidget(Qt::RightDockWidgetArea, dock);

    DataView* view{getCurrentDataView()};
    if (tabifyOn != nullptr)
        mainTab->tabifyDockWidget(tabifyOn, dock);
    else
        activateDataSelection(view);

    showPlot<T>();

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

    const auto createBasicPlot{[=]() -> BasicDataPlot* {
        DataView* view{getCurrentDataView()};
        auto* basicPlot{new BasicDataPlot()};
        connect(&(view->getPlotDataProvider()),
                &PlotDataProvider::basicPlotDataChanged, basicPlot,
                &BasicDataPlot::setNewData);
        return basicPlot;
    }};

    addPlot<BasicDataPlot>(tr("Quantiles"), createBasicPlot);
}

void TabWidget::addHistogramPlot()
{
    if (plotExist<HistogramPlotUI>())
    {
        showPlot<HistogramPlotUI>();
        return;
    }

    const auto createHistogramPlot{[=]() -> HistogramPlotUI* {
        DataView* view{getCurrentDataView()};
        auto* histogramPlot{new HistogramPlotUI()};
        connect(&(view->getPlotDataProvider()),
                &PlotDataProvider::fundamentalDataChanged, histogramPlot,
                &HistogramPlotUI::setNewData);
        return histogramPlot;
    }};

    addPlot<HistogramPlotUI>(tr("Histogram"), createHistogramPlot);
}

void TabWidget::addGroupingPlot()
{
    if (plotExist<GroupPlotUI>())
    {
        showPlot<GroupPlotUI>();
        return;
    }

    const auto createGroupingPlot{[=]() -> GroupPlotUI* {
        DataView* view{getCurrentDataView()};
        TableModel* model{getCurrentDataModel()};
        auto* groupPlot{new GroupPlotUI(getStringColumnsWithIndexes(model))};
        connect(&(view->getPlotDataProvider()),
                &PlotDataProvider::groupingPlotDataChanged, groupPlot,
                &GroupPlotUI::setNewData);
        connect(groupPlot, &GroupPlotUI::traitIndexChanged, view,
                &DataView::groupingColumnChanged);
        return groupPlot;
    }};

    addPlot<GroupPlotUI>(tr("Grouping"), createGroupingPlot);
}

void TabWidget::activateDataSelection(DataView* view)
{
    DataViewDock* viewDock{getCurrentDataViewDock()};
    viewDock->activateSelectButtons();

    view->clearSelection();
    view->setSelectionMode(QAbstractItemView::MultiSelection);
    view->selectAll();
}
