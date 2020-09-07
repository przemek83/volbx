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

FilteringProxyModel* TabWidget::getCurrentProxyModel()
{
    return getCurrentMainTab()->getCurrentProxyModel();
}

TableModel* TabWidget::getCurrentDataModel()
{
    return getCurrentMainTab()->getCurrentTableModel();
}

DataView* TabWidget::getCurrentDataView()
{
    return getCurrentMainTab()->getCurrentDataView();
}

Tab* TabWidget::getCurrentMainTab()
{
    auto currentTab{dynamic_cast<Tab*>(currentWidget())};
    return currentTab;
}

QVector<std::pair<QString, int>> TabWidget::getStringColumnsWithIndexes(
    TableModel* model) const
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

DataViewDock* TabWidget::getCurrentDataViewDock()
{
    DataView* dataView{getCurrentDataView()};
    return qobject_cast<DataViewDock*>(dataView->parent());
}

void TabWidget::setTextFilter(int column, const QStringList& bannedStrings)
{
    DataView* view{getCurrentDataView()};
    FilteringProxyModel* model{getCurrentProxyModel()};

    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();

    TimeLogger timeLogger(LogTypes::CALC, "Filtration changed");

    view->clearSelection();
    model->setStringFilter(column, bannedStrings);

    view->selectAll();
    view->recomputeAllData();

    QApplication::restoreOverrideCursor();
}

void TabWidget::setDateFilter(int column, QDate from, QDate to,
                              bool filterEmptyDates)
{
    DataView* view{getCurrentDataView()};
    FilteringProxyModel* model{getCurrentProxyModel()};

    TimeLogger timeLogger(LogTypes::CALC, "Filtration changed");

    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();

    view->clearSelection();
    model->setDateFilter(column, from, to, filterEmptyDates);

    view->selectAll();
    view->recomputeAllData();

    QApplication::restoreOverrideCursor();
}

void TabWidget::setNumericFilter(int column, double from, double to)
{
    DataView* view{getCurrentDataView()};
    FilteringProxyModel* model{getCurrentProxyModel()};

    TimeLogger timeLogger(LogTypes::CALC, "Filtration changed");

    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();

    view->clearSelection();
    model->setNumericFilter(column, from, to);

    view->selectAll();
    view->recomputeAllData();

    QApplication::restoreOverrideCursor();
}

template <class T>
void TabWidget::addPlot(const QString& title,
                        const std::function<T*()>& createPlot)
{
    DataView* view{getCurrentDataView()};
    Tab* mainTab{getCurrentMainTab()};
    TableModel* model{getCurrentDataModel()};
    if (view == nullptr || model == nullptr || mainTab == nullptr)
        return;

    // If plot already created than just show it and return.
    if (auto plotUI{mainTab->findChild<T*>()}; plotUI != nullptr)
    {
        auto dock{qobject_cast<PlotDock*>(plotUI->parent())};
        dock->setVisible(true);
        dock->raise();
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();

    auto tabifyOn{mainTab->findChild<PlotDock*>()};
    auto dock{new PlotDock(title, mainTab)};
    dock->setWidget(createPlot());
    mainTab->addDockWidget(Qt::RightDockWidgetArea, dock);

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
    const auto createBasicPlot{[=]() -> BasicDataPlot* {
        DataView* view{getCurrentDataView()};
        auto basicPlot{new BasicDataPlot()};
        connect(&(view->getPlotDataProvider()),
                &PlotDataProvider::basicPlotDataChanged, basicPlot,
                &BasicDataPlot::setNewData);
        return basicPlot;
    }};

    addPlot<BasicDataPlot>(tr("Quantiles"), createBasicPlot);
}

void TabWidget::addHistogramPlot()
{
    const auto createHistogramPlot{[=]() -> HistogramPlotUI* {
        DataView* view{getCurrentDataView()};
        auto histogramPlot{new HistogramPlotUI()};
        connect(&(view->getPlotDataProvider()),
                &PlotDataProvider::fundamentalDataChanged, histogramPlot,
                &HistogramPlotUI::setNewData);
        return histogramPlot;
    }};

    addPlot<HistogramPlotUI>(tr("Histogram"), createHistogramPlot);
}

void TabWidget::addGroupingPlot()
{
    const auto createGroupingPlot{[=]() -> GroupPlotUI* {
        DataView* view{getCurrentDataView()};
        TableModel* model{getCurrentDataModel()};
        auto groupPlot{new GroupPlotUI(getStringColumnsWithIndexes(model))};
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
