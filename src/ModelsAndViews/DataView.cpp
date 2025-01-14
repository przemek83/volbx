#include "DataView.h"

#include <QApplication>
#include <QHeaderView>
#include <QMouseEvent>

#include <Common/Constants.h>
#include <Common/TimeLogger.h>

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
    const auto* proxyModel{qobject_cast<FilteringProxyModel*>(model)};
    const TableModel* parentModel{proxyModel->getParentModel()};

    for (int column = 0; column < proxyModel->columnCount(); ++column)
        setDelegate(column, parentModel);

    QTableView::setModel(model);
    groupByColumn_ = parentModel->getDefaultGroupingColumn();
}

void DataView::groupingColumnChanged(int column)
{
    groupByColumn_ = column;
    const TableModel* parentModel{getParentModel()};
    plotDataProvider_.recomputeGroupingData(
        fillDataFromSelection(column), parentModel->getColumnFormat(column));
}

std::tuple<bool, int, int> DataView::getTaggedColumns(
    const TableModel* parentModel)
{
    int pricePerMeterColumn{Constants::NOT_SET_COLUMN};
    if (auto [ok, columnId] =
            parentModel->getTaggedColumnIfExists(ColumnTag::VALUE);
        ok)
        pricePerMeterColumn = columnId;
    else
        return {false, Constants::NOT_SET_COLUMN, Constants::NOT_SET_COLUMN};

    int transactionDateColumn{Constants::NOT_SET_COLUMN};
    if (auto [ok, columnId] =
            parentModel->getTaggedColumnIfExists(ColumnTag::DATE);
        ok)
        transactionDateColumn = columnId;
    else
        return {false, Constants::NOT_SET_COLUMN, Constants::NOT_SET_COLUMN};
    return {true, pricePerMeterColumn, transactionDateColumn};
}

void DataView::setDelegate(int columnIndex, const TableModel* parentModel)
{
    switch (parentModel->getColumnFormat(columnIndex))
    {
        case ColumnType::NUMBER:
        {
            auto* delegate{new NumericDelegate(this)};
            setItemDelegateForColumn(columnIndex, delegate);
            break;
        }

        case ColumnType::DATE:
        {
            auto* delegate{new DateDelegate(this)};
            setItemDelegateForColumn(columnIndex, delegate);
            break;
        }

        case ColumnType::STRING:
        case ColumnType::UNKNOWN:
        {
            break;
        }
    }
}

QVector<TransactionData> DataView::fillDataFromSelection(
    int groupByColumn) const
{
    const TableModel* parentModel{getParentModel()};

    const auto [success, pricePerMeterColumn, transactionDateColumn] =
        getTaggedColumns(parentModel);
    if (!success)
        return {};

    const TimeLogger timeLogger(LogTypes::CALC, QStringLiteral("Data updated"));

    const QItemSelectionModel* selectionModelOfView{selectionModel()};
    QVector<TransactionData> calcDataContainer;
    const int batchSize{1000};

    const FilteringProxyModel* proxyModel{getProxyModel()};
    for (int i = 0; i < proxyModel->rowCount(); ++i)
    {
        if (i % batchSize == 0)
            QApplication::processEvents();

        if (!selectionModelOfView->isSelected(proxyModel->index(i, 0)))
            continue;
        const QVariant& dateVariant{
            proxyModel->index(i, transactionDateColumn).data()};
        if (dateVariant.isNull())
            continue;

        TransactionData transactionData;
        transactionData.date_ = dateVariant.toDate();
        transactionData.pricePerMeter_ =
            proxyModel->index(i, pricePerMeterColumn).data().toDouble();

        if (groupByColumn != Constants::NOT_SET_COLUMN)
            transactionData.groupedBy_ =
                proxyModel->index(i, groupByColumn).data();

        calcDataContainer.append(transactionData);
    }

    return calcDataContainer;
}

void DataView::recomputeAllData()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QApplication::processEvents();

    ColumnType columnFormat{ColumnType::UNKNOWN};
    if (groupByColumn_ != Constants::NOT_SET_COLUMN)
    {
        const TableModel* parentModel{getParentModel()};
        columnFormat = parentModel->getColumnFormat(groupByColumn_);
    }

    const TimeLogger timeLogger(LogTypes::CALC,
                                QStringLiteral("Plots recomputed"));

    plotDataProvider_.recompute(fillDataFromSelection(groupByColumn_),
                                columnFormat);

    QApplication::restoreOverrideCursor();
}

void DataView::mouseReleaseEvent(QMouseEvent* event)
{
    QTableView::mouseReleaseEvent(event);
    if (event->button() == Qt::LeftButton)
        recomputeAllData();
}

void DataView::keyPressEvent(QKeyEvent* event)
{
    QTableView::keyPressEvent(event);
    if (event->key() == Qt::Key_A && event->modifiers() == Qt::CTRL)
        recomputeAllData();
}

const PlotDataProvider& DataView::getPlotDataProvider() const
{
    return plotDataProvider_;
}

void DataView::initHorizontalHeader()
{
    horizontalHeader()->setSortIndicatorShown(false);
    horizontalHeader()->setSectionsMovable(true);

    auto showSortIndicator = [=]()
    {
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
