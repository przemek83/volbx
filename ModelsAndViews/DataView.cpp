#include "DataView.h"

#include <QApplication>
#include <QHeaderView>
#include <QMouseEvent>

#include "Logger.h"

#include "DateDelegate.h"
#include "FilteringProxyModel.h"
#include "NumericDelegate.h"
#include "TableModel.h"

DataView::DataView(QWidget* parent)
    : QTableView(parent), plotDataProvider_(this)
{
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSortingEnabled(true);

    initHorizontalHeader();
    initVerticalHeader();
}

void DataView::setModel(QAbstractItemModel* model)
{
    const auto proxyModel = qobject_cast<FilteringProxyModel*>(model);
    const TableModel* parentModel = proxyModel->getParentModel();

    for (int i = 0; i < proxyModel->columnCount(); ++i)
    {
        switch (parentModel->getColumnFormat(i))
        {
            case ColumnType::NUMBER:
            {
                setItemDelegateForColumn(i, new NumericDelegate(this));
                break;
            }

            case ColumnType::DATE:
            {
                setItemDelegateForColumn(i, new DateDelegate(this));
                break;
            }

            case ColumnType::STRING:
            case ColumnType::UNKNOWN:
            {
                break;
            }
        }
    }

    QTableView::setModel(model);

    plotDataProvider_.setGroupingColumn(
        parentModel->getDefaultGroupingColumn());
}

void DataView::groupingColumnChanged(int column)
{
    const TableModel* parentModel{getParentModel()};
    plotDataProvider_.recomputeGroupData(fillDataFromSelection(column), column,
                                         parentModel->getColumnFormat(column));
}

QVector<TransactionData> DataView::fillDataFromSelection(
    int groupByColumn) const
{
    const FilteringProxyModel* proxyModel{getProxyModel()};
    const TableModel* parentModel{getParentModel()};

    int pricePerMeterColumn;
    if (auto [ok, columnId] = parentModel->getSpecialColumnIfExists(
            SpecialColumn::PRICE_PER_UNIT);
        ok)
        pricePerMeterColumn = columnId;
    else
        return {};

    int transactionDateColumn;
    if (auto [ok, columnId] = parentModel->getSpecialColumnIfExists(
            SpecialColumn::TRANSACTION_DATE);
        ok)
        transactionDateColumn = columnId;
    else
        return {};

    QTime performanceTimer;
    performanceTimer.start();

    QItemSelectionModel* selectionModelOfView = selectionModel();
    QVector<TransactionData> calcDataContainer;
    const int proxyRowCount = proxyModel->rowCount();
    const int batchSize{1000};
    for (int i = 0; i < proxyRowCount; ++i)
    {
        if (i % batchSize == 0)
            QApplication::processEvents();

        if (!selectionModelOfView->isSelected(proxyModel->index(i, 0)))
            continue;

        TransactionData temp;
        const QVariant& data =
            proxyModel->index(i, transactionDateColumn).data();

        // Do not take into calculations and plots rows with empty date or
        // price.
        if (!data.isNull())
        {
            temp.pricePerMeter_ =
                proxyModel->index(i, pricePerMeterColumn).data().toDouble();
            temp.date_ = data.toDate();

            // String values can be stored as indexes of table
            // with data when it will be done.

            // Temp, remove when all types of column managed in grouping.
            if (groupByColumn != Constants::NOT_SET_COLUMN)
                temp.groupedBy_ = proxyModel->index(i, groupByColumn).data();

            calcDataContainer.append(temp);
        }
    }

    LOG(LogTypes::CALC,
        "Data updated in time " +
            QString::number(performanceTimer.elapsed() * 1.0 / 1000) +
            " seconds.");

    return calcDataContainer;
}

void DataView::reloadSelectionDataAndRecompute()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();

    // TODO optimize by impact + depact or additional columns in model.
    const int groupByColumn = plotDataProvider_.getGroupByColumn();
    QVector<TransactionData> newCalcData = fillDataFromSelection(groupByColumn);

    // Temp, until all column types managed.
    ColumnType columnFormat = ColumnType::UNKNOWN;
    if (groupByColumn != Constants::NOT_SET_COLUMN)
    {
        const TableModel* parentModel{getParentModel()};
        columnFormat = parentModel->getColumnFormat(groupByColumn);
    }

    QTime performanceTimer;
    performanceTimer.start();

    plotDataProvider_.reCompute(std::move(newCalcData), columnFormat);

    LOG(LogTypes::CALC,
        "Plots recomputed in " +
            QString::number(performanceTimer.elapsed() * 1.0 / 1000) +
            " seconds.");

    QApplication::restoreOverrideCursor();
}

void DataView::mouseReleaseEvent(QMouseEvent* event)
{
    QTableView::mouseReleaseEvent(event);

    if (Qt::LeftButton == event->button())
        reloadSelectionDataAndRecompute();
}

void DataView::keyPressEvent(QKeyEvent* event)
{
    QTableView::keyPressEvent(event);

    if (Qt::Key_A == event->key() && Qt::CTRL == event->modifiers())
        reloadSelectionDataAndRecompute();
}

const PlotDataProvider& DataView::getPlotDataProvider() const
{
    return plotDataProvider_;
}

void DataView::initHorizontalHeader()
{
    horizontalHeader()->setSortIndicatorShown(false);
    horizontalHeader()->setSectionsMovable(true);

    auto showSortIndicator = [=]() {
        if (!horizontalHeader()->isSortIndicatorShown())
            horizontalHeader()->setSortIndicatorShown(true);
    };
    connect(horizontalHeader(), &QHeaderView::sectionClicked,
            showSortIndicator);
}

void DataView::initVerticalHeader()
{
    const int rowHeight{static_cast<int>(fontMetrics().height() * 1.5)};
    verticalHeader()->setDefaultSectionSize(rowHeight);
    verticalHeader()->setVisible(false);
}

const FilteringProxyModel* DataView::getProxyModel() const
{
    return qobject_cast<FilteringProxyModel*>(model());
}

const TableModel* DataView::getParentModel() const
{
    return getProxyModel()->getParentModel();
}
