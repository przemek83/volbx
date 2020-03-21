#include "MainTabWidget.h"

#include <BasicDataPlot.h>
#include <QApplication>
#include <QDebug>

#include "DataProvider/PlotDataProvider.h"
#include "ModelsAndViews/DataView.h"
#include "ModelsAndViews/FilteringProxyModel.h"
#include "ModelsAndViews/TableModel.h"
#include "Shared/Logger.h"

#include "GroupPlotGui.h"
#include "HistogramPlotUI.h"
#include "MainTab.h"
#include "PlotDockWidget.h"
#include "TabBar.h"
#include "ViewDockWidget.h"

MainTabWidget::MainTabWidget(QWidget* parent) :
    QTabWidget(parent)
{
    setTabBar(new TabBar(this));
    setTabsClosable(true);
    setMovable(true);
}

FilteringProxyModel* MainTabWidget::getCurrentProxyModel()
{
    auto currentTab = dynamic_cast<MainTab*>(currentWidget());
    Q_ASSERT(nullptr != currentTab);
    if (nullptr == currentTab)
    {
        return nullptr;
    }
    return currentTab->getCurrentProxyModel();
}

TableModel* MainTabWidget::getCurrentDataModel()
{
    auto currentTab = dynamic_cast<MainTab*>(currentWidget());
    Q_ASSERT(nullptr != currentTab);
    if (nullptr == currentTab)
    {
        return nullptr;
    }
    return currentTab->getCurrentDataModel();
}

DataView* MainTabWidget::getCurrentDataView()
{
    auto currentTab = dynamic_cast<MainTab*>(currentWidget());
    Q_ASSERT(nullptr != currentTab);
    if (nullptr == currentTab)
    {
        return nullptr;
    }
    return currentTab->getCurrentDataView();
}

MainTab* MainTabWidget::getCurrentMainTab()
{
    auto currentTab = dynamic_cast<MainTab*>(currentWidget());
    Q_ASSERT(nullptr != currentTab);
    return currentTab;
}

QVector<std::pair<QString, int>> MainTabWidget::getStringColumnsWithIndexes(TableModel* model) const
{
    QVector<std::pair<QString, int>> stringColumns;
    for (int i = 0; i < model->columnCount(); ++i)
    {
        // Accept only string type columns.
        if (DATA_FORMAT_STRING != model->getColumnFormat(i))
            continue;

        const QString columnName =
            model->headerData(i, Qt::Horizontal).toString();
        stringColumns.append({columnName, i});
    }
    return stringColumns;
}

ViewDockWidget* MainTabWidget::getCurrentDataViewDock()
{
    DataView* dataView = getCurrentDataView();
    Q_ASSERT(nullptr != dataView);
    if (nullptr == dataView)
    {
        return nullptr;
    }
    return qobject_cast<ViewDockWidget*>(dataView->parent());
}

void MainTabWidget::setTextFilterInProxy(int column, const QStringList& bannedStrings)
{
    DataView* view = getCurrentDataView();
    FilteringProxyModel* model = getCurrentProxyModel();
    if (nullptr == view || nullptr == model)
    {
        return;
    }

    QTime performanceTimer;
    performanceTimer.start();

    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();

    view->clearSelection();

    model->setStringFilter(column, bannedStrings);

    view->selectAll();
    view->reloadSelectionDataAndRecompute();

    QApplication::restoreOverrideCursor();

    LOG(LogTypes::CALC, "Filtration change took " +
        QString::number(performanceTimer.elapsed() * 1.0 / 1000) +
        " seconds.");
}

void MainTabWidget::setDateFilterInProxy(int column,
                                         QDate from,
                                         QDate to,
                                         bool filterEmptyDates)
{
    DataView* view = getCurrentDataView();
    FilteringProxyModel* model = getCurrentProxyModel();
    if (nullptr == view || nullptr == model)
    {
        return;
    }

    QTime performanceTimer;
    performanceTimer.start();

    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();
    view->clearSelection();
    model->setDateFilter(column, from, to, filterEmptyDates);

    view->selectAll();
    view->reloadSelectionDataAndRecompute();

    QApplication::restoreOverrideCursor();

    LOG(LogTypes::CALC, "Filtration change took " +
        QString::number(performanceTimer.elapsed() * 1.0 / 1000) +
        " seconds.");
}

void MainTabWidget::setNumericFilterInProxy(int column, double from, double to)
{
    DataView* view = getCurrentDataView();
    FilteringProxyModel* model = getCurrentProxyModel();
    if (nullptr == view || nullptr == model)
    {
        return;
    }

    QTime performanceTimer;
    performanceTimer.start();

    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();

    view->clearSelection();
    model->setNumericFilter(column, from, to);

    view->selectAll();
    view->reloadSelectionDataAndRecompute();

    QApplication::restoreOverrideCursor();

    LOG(LogTypes::CALC, "Filtration change took " +
        QString::number(performanceTimer.elapsed() * 1.0 / 1000) +
        " seconds.");
}

void MainTabWidget::addBasicPlot()
{
    DataView* view = getCurrentDataView();
    MainTab* mainTab = getCurrentMainTab();
    if (nullptr == view /*|| nullptr == model*/ || nullptr == mainTab)
    {
        return;
    }

    //If basic data plot already created than just show it and return.
    auto basicDataPlot = mainTab->findChild<BasicDataPlot*>();
    if (nullptr != basicDataPlot)
    {
        auto basicPlotDock =
            qobject_cast<PlotDockWidget*>(basicDataPlot->parent());
        Q_ASSERT(nullptr != basicPlotDock);
        if (nullptr != basicPlotDock)
        {
            basicPlotDock->setVisible(true);
            basicPlotDock->raise();
            return;
        }
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();

    auto tabifyOn = mainTab->findChild<PlotDockWidget*>();
    auto plotDock = new PlotDockWidget(QStringLiteral("Quantiles"), mainTab);
    basicDataPlot = new BasicDataPlot(plotDock);
    plotDock->setWidget(basicDataPlot);
    mainTab->addDockWidget(Qt::RightDockWidgetArea, plotDock);
    if (nullptr != tabifyOn)
    {
        plotDock->setVisible(false);
        mainTab->tabifyDockWidget(tabifyOn, plotDock);
    }
    else
    {
        //For first plot.
        activateDataSelection(view);
    }

    connect(view->getPlotDataProvider(), &PlotDataProvider::basicPlotDataChanged,
            basicDataPlot, &BasicDataPlot::setNewData);

    view->reloadSelectionDataAndRecompute();

    //Problem with blinking display. Workaround used.
    if (nullptr != tabifyOn)
    {
        plotDock->setVisible(true);
        plotDock->raise();
    }

    QApplication::restoreOverrideCursor();
}

void MainTabWidget::addHistogramPlot()
{
    DataView* view = getCurrentDataView();
    MainTab* mainTab = getCurrentMainTab();
    TableModel* model = getCurrentDataModel();
    if (view == nullptr || model == nullptr || mainTab == nullptr)
        return;

    // If plot already created than just show it and return.
    if (auto plotUI = mainTab->findChild<HistogramPlotUI*>(); plotUI != nullptr)
    {
        auto dock {static_cast<PlotDockWidget*>(plotUI->parent())};
        dock->setVisible(true);
        dock->raise();
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();

    auto tabifyOn = mainTab->findChild<PlotDockWidget*>();
    auto histogramPlotUI = new HistogramPlotUI();
    connect(view->getPlotDataProvider(), &PlotDataProvider::basicDataChanged,
            histogramPlotUI, &HistogramPlotUI::dataChanged);

    auto histogramDock = new PlotDockWidget(tr("Histogram"), mainTab);
    histogramDock->setWidget(histogramPlotUI);
    mainTab->addDockWidget(Qt::RightDockWidgetArea, histogramDock);

    if (tabifyOn != nullptr)
    {
        histogramDock->setVisible(false);
        mainTab->tabifyDockWidget(tabifyOn, histogramDock);
    }
    else
        activateDataSelection(view);

    view->reloadSelectionDataAndRecompute();
    QApplication::restoreOverrideCursor();
}

void MainTabWidget::addGroupingPlot()
{
    DataView* view = getCurrentDataView();
    MainTab* mainTab = getCurrentMainTab();
    TableModel* model = getCurrentDataModel();
    if (nullptr == view || nullptr == model || nullptr == mainTab)
    {
        return;
    }

    //If basic data plot already created than just show it and return.
    auto groupPlotGui = mainTab->findChild<GroupPlotGui*>();
    if (nullptr != groupPlotGui)
    {
        groupPlotGui->setVisible(true);
        groupPlotGui->raise();
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();

    auto tabifyOn = mainTab->findChild<PlotDockWidget*>();

    groupPlotGui = new GroupPlotGui(getStringColumnsWithIndexes(model), mainTab);
    mainTab->addDockWidget(Qt::RightDockWidgetArea, groupPlotGui);
    if (nullptr != tabifyOn)
    {
        groupPlotGui->setVisible(false);
        mainTab->tabifyDockWidget(tabifyOn, groupPlotGui);
    }
    else
    {
        //For first plot.
        activateDataSelection(view);
    }

    connect(view->getPlotDataProvider(), &PlotDataProvider::setNewDataForGrouping,
            groupPlotGui, &GroupPlotGui::setNewData);

    connect(groupPlotGui, &GroupPlotGui::newGroupingColumn,
            view, &DataView::groupingColumnChanged);

    view->reloadSelectionDataAndRecompute();

    //Problem with blinking display. Workaround used.
    if (nullptr != tabifyOn)
    {
        groupPlotGui->setVisible(true);
        groupPlotGui->raise();
    }

    QApplication::restoreOverrideCursor();
}

void MainTabWidget::activateDataSelection(DataView* view)
{
    //Activate select all and unselect all buttons on data view dock.
    ViewDockWidget* viewDock = getCurrentDataViewDock();
    if (viewDock == nullptr)
        return;

    viewDock->activateSelectButtons();

    view->clearSelection();
    view->setSelectionMode(QAbstractItemView::MultiSelection);
    view->selectAll();
}
