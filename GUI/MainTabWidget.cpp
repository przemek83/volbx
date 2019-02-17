#include <QApplication>
#include <QDebug>

#include "ModelsAndViews/TableModel.h"
#include "ModelsAndViews/DataView.h"
#include "ModelsAndViews/FilteringProxyModel.h"
#include "Shared/Logger.h"
#include "DataProvider/PlotDataProvider.h"
#include "Charts/BasicDataPlot.h"
#include "Charts/QuantilesPlot.h"

#include "TabBar.h"
#include "MainTabWidget.h"
#include "MainTab.h"
#include "PlotDockWidget.h"
#include "HistogramPlotGui.h"
#include "GroupPlotGui.h"
#include "ViewDockWidget.h"

MainTabWidget::MainTabWidget(QWidget *parent) :
    QTabWidget(parent)
{
    setTabBar(new TabBar(this));
    setTabsClosable(true);
    setMovable(true);
}

MainTabWidget::~MainTabWidget()
{

}

FilteringProxyModel* MainTabWidget::getCurrentProxyModel()
{
    MainTab* currentTab = static_cast<MainTab*>(currentWidget());
    Q_ASSERT(nullptr != currentTab);
    if( nullptr == currentTab )
    {
        return nullptr;
    }
    return currentTab->getCurrentProxyModel();
}

TableModel* MainTabWidget::getCurrentDataModel()
{
    MainTab* currentTab = static_cast<MainTab*>(currentWidget());
    Q_ASSERT(nullptr != currentTab);
    if( nullptr == currentTab )
    {
        return nullptr;
    }
    return currentTab->getCurrentDataModel();
}

DataView* MainTabWidget::getCurrentDataView()
{
    MainTab* currentTab = static_cast<MainTab*>(currentWidget());
    Q_ASSERT(nullptr != currentTab);
    if( nullptr == currentTab )
    {
        return nullptr;
    }
    return currentTab->getCurrentDataView();
}

MainTab* MainTabWidget::getCurrentMainTab()
{
    MainTab* currentTab = static_cast<MainTab*>(currentWidget());
    Q_ASSERT(nullptr != currentTab);
    return currentTab;
}

ViewDockWidget* MainTabWidget::getCurrentDataViewDock()
{
    DataView* dataView = getCurrentDataView();
    Q_ASSERT(nullptr != dataView);
    if( nullptr == dataView )
    {
        return nullptr;
    }
    return dynamic_cast<ViewDockWidget*>(dataView->parent());
}

void MainTabWidget::setTextFilterInProxy(int column, QSet<QString> bannedStrings)
{
    DataView* view = getCurrentDataView();
    FilteringProxyModel* model = getCurrentProxyModel();
    if ( nullptr == view || nullptr == model )
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

    LOG(LOG_CALC, "Filtration change took " +
                  QString::number(performanceTimer.elapsed()*1.0/1000) +
                  " seconds.");
}

void MainTabWidget::setDateFilterInProxy(int column,
                                         QDate from,
                                         QDate to,
                                         bool filterEmptyDates)
{
    DataView* view = getCurrentDataView();
    FilteringProxyModel* model = getCurrentProxyModel();
    if ( nullptr == view || nullptr == model )
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

    LOG(LOG_CALC, "Filtration change took " +
                  QString::number(performanceTimer.elapsed()*1.0/1000) +
                  " seconds.");
}

void MainTabWidget::setNumericFilterInProxy(int column, double from, double to)
{
    DataView* view = getCurrentDataView();
    FilteringProxyModel* model = getCurrentProxyModel();
    if ( nullptr == view || nullptr == model )
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

    LOG(LOG_CALC, "Filtration change took " +
                  QString::number(performanceTimer.elapsed()*1.0/1000) +
                  " seconds.");
}

void MainTabWidget::addBasicPlot()
{
    DataView* view = getCurrentDataView();
    MainTab* mainTab = getCurrentMainTab();
    if ( nullptr == view /*|| nullptr == model*/ || nullptr == mainTab )
    {
        return;
    }

    //If basic data plot already created than just show it and return.
    BasicDataPlot* basicDataPlot = mainTab->findChild<BasicDataPlot*>();
    if( nullptr != basicDataPlot )
    {
        PlotDockWidget* basicPlotDock =
            dynamic_cast<PlotDockWidget*>(basicDataPlot->parent());
        Q_ASSERT(nullptr != basicPlotDock);
        if( nullptr != basicPlotDock )
        {
            basicPlotDock->setVisible(true);
            basicPlotDock->raise();
            return;
        }
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();

    PlotDockWidget* tabifyOn = mainTab->findChild<PlotDockWidget*>();
    PlotDockWidget* plotDock = new PlotDockWidget("Quantiles", mainTab);
    basicDataPlot = new BasicDataPlot(plotDock);
    plotDock->setWidget(basicDataPlot);
    mainTab->addDockWidget(Qt::RightDockWidgetArea, plotDock);
    if( nullptr != tabifyOn )
    {
        plotDock->setVisible(false);
        mainTab->tabifyDockWidget(tabifyOn, plotDock);
    }
    else
    {
        //For first plot.
        changeDataViewMode(view);
    }

    connect(view->getPlotDataProvider(),
            SIGNAL(basicPlotDataChanged(PlotData,Quantiles,QVector<QPointF>)),
            basicDataPlot,
            SLOT(setNewData(PlotData,Quantiles,QVector<QPointF>)));

    view->reloadSelectionDataAndRecompute();

    //Problem with blinking display. Workaround used.
    if( nullptr != tabifyOn )
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
    if ( nullptr == view || nullptr == model || nullptr == mainTab )
    {
        return;
    }

    //If basic data plot already created than just show it and return.
    HistogramPlotGui* histogramPlotGui =
        mainTab->findChild<HistogramPlotGui*>();
    if( nullptr != histogramPlotGui )
    {
        histogramPlotGui->setVisible(true);
        histogramPlotGui->raise();
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();

    PlotDockWidget* tabifyOn = mainTab->findChild<PlotDockWidget*>();
    histogramPlotGui = new HistogramPlotGui(mainTab);
    mainTab->addDockWidget(Qt::RightDockWidgetArea, histogramPlotGui);
    if( nullptr != tabifyOn )
    {
        histogramPlotGui->setVisible(false);
        mainTab->tabifyDockWidget(tabifyOn, histogramPlotGui);
    }
    else
    {
        //For first plot.
        changeDataViewMode(view);
    }

    connect(view->getPlotDataProvider(),
            SIGNAL(basicDataChanged(PlotData,Quantiles)),
            histogramPlotGui,
            SLOT(dataChanged(PlotData,Quantiles)));

    view->reloadSelectionDataAndRecompute();

    //Problem with blinking display. Workaround used.
    if( nullptr != tabifyOn )
    {
        histogramPlotGui->setVisible(true);
        histogramPlotGui->raise();
    }

    QApplication::restoreOverrideCursor();

}

void MainTabWidget::addGroupingPlot()
{
    DataView* view = getCurrentDataView();
    MainTab* mainTab = getCurrentMainTab();
    TableModel* model = getCurrentDataModel();
    if ( nullptr == view || nullptr == model || nullptr == mainTab )
    {
        return;
    }

    //If basic data plot already created than just show it and return.
    GroupPlotGui* groupPlotGui = mainTab->findChild<GroupPlotGui*>();
    if( nullptr != groupPlotGui )
    {
        groupPlotGui->setVisible(true);
        groupPlotGui->raise();
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();

    PlotDockWidget* tabifyOn = mainTab->findChild<PlotDockWidget*>();
    groupPlotGui = new GroupPlotGui(model, mainTab);
    mainTab->addDockWidget(Qt::RightDockWidgetArea, groupPlotGui);
    if( nullptr != tabifyOn )
    {
        groupPlotGui->setVisible(false);
        mainTab->tabifyDockWidget(tabifyOn, groupPlotGui);
    }
    else
    {
        //For first plot.
        changeDataViewMode(view);
    }

    connect(view->getPlotDataProvider(),
            SIGNAL(setNewDataForGrouping(float,
                                         float,
                                         QVector<QString>,
                                         QVector<Quantiles>,
                                         Quantiles)),
            groupPlotGui,
            SLOT(setNewData(float,
                            float,
                            QVector<QString>,
                            QVector<Quantiles>,
                            Quantiles)));

    connect(groupPlotGui,
            SIGNAL(newGroupingColumn(int)),
            view,
            SLOT(groupingColumnChanged(int)));

    view->reloadSelectionDataAndRecompute();

    //Problem with blinking display. Workaround used.
    if( nullptr != tabifyOn )
    {
        groupPlotGui->setVisible(true);
        groupPlotGui->raise();
    }

    QApplication::restoreOverrideCursor();
}

void MainTabWidget::changeDataViewMode(DataView* view)
{
    //Activate select all and unselect all buttons on data view dock.
    ViewDockWidget* viewDock = getCurrentDataViewDock();
    if( nullptr == viewDock )
    {
        return;
    }
    viewDock->activateSelectButtons();

    view->clearSelection();
    view->setSelectionMode(QAbstractItemView::MultiSelection);
    view->selectAll();
}
