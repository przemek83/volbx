#include "MainTabWidget.h"

#include <BasicDataPlot.h>
#include <GroupPlotUI.h>
#include <HistogramPlotUI.h>
#include <QApplication>
#include <QDebug>

#include "DataProvider/PlotDataProvider.h"
#include "ModelsAndViews/DataView.h"
#include "ModelsAndViews/FilteringProxyModel.h"
#include "ModelsAndViews/TableModel.h"
#include "Shared/Logger.h"

#include "MainTab.h"
#include "PlotDockWidget.h"
#include "TabBar.h"
#include "ViewDockWidget.h"

MainTabWidget::MainTabWidget(QWidget* parent) : QTabWidget(parent)
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

QVector<std::pair<QString, int>> MainTabWidget::getStringColumnsWithIndexes(
    TableModel* model) const
{
    QVector<std::pair<QString, int>> stringColumns;
    for (int i = 0; i < model->columnCount(); ++i)
    {
        // Accept only string type columns.
        if (ColumnType::STRING != model->getColumnFormat(i))
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

void MainTabWidget::setTextFilterInProxy(int column,
                                         const QStringList& bannedStrings)
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

    LOG(LogTypes::CALC,
        "Filtration change took " +
            QString::number(performanceTimer.elapsed() * 1.0 / 1000) +
            " seconds.");
}

void MainTabWidget::setDateFilterInProxy(int column, QDate from, QDate to,
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

    LOG(LogTypes::CALC,
        "Filtration change took " +
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

    LOG(LogTypes::CALC,
        "Filtration change took " +
            QString::number(performanceTimer.elapsed() * 1.0 / 1000) +
            " seconds.");
}

template <class T>
void MainTabWidget::addPlot(const QString& title,
                            const std::function<T*()>& createPlot)
{
    DataView* view = getCurrentDataView();
    MainTab* mainTab = getCurrentMainTab();
    TableModel* model = getCurrentDataModel();
    if (view == nullptr || model == nullptr || mainTab == nullptr)
        return;

    // If plot already created than just show it and return.
    if (auto plotUI = mainTab->findChild<T*>(); plotUI != nullptr)
    {
        auto dock{qobject_cast<PlotDockWidget*>(plotUI->parent())};
        dock->setVisible(true);
        dock->raise();
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();

    auto tabifyOn = mainTab->findChild<PlotDockWidget*>();
    auto dock = new PlotDockWidget(title, mainTab);
    dock->setWidget(createPlot());
    mainTab->addDockWidget(Qt::RightDockWidgetArea, dock);

    if (tabifyOn != nullptr)
        mainTab->tabifyDockWidget(tabifyOn, dock);
    else
        activateDataSelection(view);

    dock->setVisible(true);
    dock->raise();

    view->reloadSelectionDataAndRecompute();
    QApplication::restoreOverrideCursor();
}

void MainTabWidget::addBasicPlot()
{
    const auto createBasicPlot = [=]() -> BasicDataPlot* {
        DataView* view = getCurrentDataView();
        auto basicPlot = new BasicDataPlot();
        connect(view->getPlotDataProvider(),
                &PlotDataProvider::basicPlotDataChanged, basicPlot,
                &BasicDataPlot::setNewData);
        return basicPlot;
    };

    addPlot<BasicDataPlot>(tr("Quantiles"), createBasicPlot);
}

void MainTabWidget::addHistogramPlot()
{
    const auto createHistogramPlot = [=]() -> HistogramPlotUI* {
        DataView* view = getCurrentDataView();
        auto histogramPlot = new HistogramPlotUI();
        connect(view->getPlotDataProvider(),
                &PlotDataProvider::basicDataChanged, histogramPlot,
                &HistogramPlotUI::setNewData);
        return histogramPlot;
    };

    addPlot<HistogramPlotUI>(tr("Histogram"), createHistogramPlot);
}

void MainTabWidget::addGroupingPlot()
{
    const auto createGroupingPlot = [=]() -> GroupPlotUI* {
        DataView* view = getCurrentDataView();
        TableModel* model = getCurrentDataModel();
        auto groupPlot = new GroupPlotUI(getStringColumnsWithIndexes(model));
        connect(view->getPlotDataProvider(),
                &PlotDataProvider::setNewDataForGrouping, groupPlot,
                &GroupPlotUI::setNewData);
        connect(groupPlot, &GroupPlotUI::traitIndexChanged, view,
                &DataView::groupingColumnChanged);
        return groupPlot;
    };

    addPlot<GroupPlotUI>(tr("Grouping"), createGroupingPlot);
}

void MainTabWidget::activateDataSelection(DataView* view)
{
    // Activate select all and unselect all buttons on data view dock.
    ViewDockWidget* viewDock = getCurrentDataViewDock();
    if (viewDock == nullptr)
        return;

    viewDock->activateSelectButtons();

    view->clearSelection();
    view->setSelectionMode(QAbstractItemView::MultiSelection);
    view->selectAll();
}
