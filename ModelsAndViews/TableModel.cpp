#include "TableModel.h"

#include <QDebug>

#include "Common/Constants.h"
#include "Datasets/Dataset.h"

const int TableModel::noColumn_ = Constants::NOT_SET_COLUMN;

TableModel::TableModel(std::unique_ptr<Dataset> dataset, QObject* parent)
    : QAbstractTableModel(parent), dataset_(std::move(dataset))
{
}

int TableModel::rowCount([[maybe_unused]] const QModelIndex& parent) const
{
    return dataset_->rowCount();
}

int TableModel::columnCount([[maybe_unused]] const QModelIndex& parent) const
{
    return dataset_->columnCount();
}

QVariant TableModel::data(const QModelIndex& index, int role) const
{
    if (Qt::DisplayRole == role)
    {
        return *dataset_->getData(index.row(), index.column());
    }

    return QVariant();
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation,
                                int role) const
{
    if (Qt::DisplayRole == role && Qt::Horizontal == orientation)
    {
        return dataset_->getHeaderName(section);
    }
    return QVariant();
}

std::tuple<double, double> TableModel::getNumericRange(int column) const
{
    return dataset_->getNumericRange(column);
}

std::tuple<QDate, QDate, bool> TableModel::getDateRange(int column) const
{
    return dataset_->getDateRange(column);
}

QStringList TableModel::getStringList(int column) const
{
    return dataset_->getStringList(column);
}

ColumnType TableModel::getColumnFormat(int column) const
{
    return dataset_->getColumnFormat(column);
}

std::tuple<bool, int> TableModel::getSpecialColumnIfExists(
    SpecialColumn columnTag) const
{
    return dataset_->getSpecialColumn(columnTag);
}

QByteArray TableModel::definitionToXml(int rowCount) const
{
    return dataset_->definitionToXml(rowCount);
}

bool TableModel::isSpecialColumnsSet() const
{
    auto [transDateColumnSet, transDateColumnId] =
        getSpecialColumnIfExists(SpecialColumn::TRANSACTION_DATE);
    auto [priceColumnSet, priceColumnId] =
        getSpecialColumnIfExists(SpecialColumn::PRICE_PER_UNIT);

    return transDateColumnSet && priceColumnSet;
}

bool TableModel::isEmptyCellsDetected() const
{
    // TODO or delete.
    return true;
}

int TableModel::getDefaultGroupingColumn() const
{
    int pricePerMeterColumn = noColumn_;
    if (auto [ok, columnId] =
            getSpecialColumnIfExists(SpecialColumn::PRICE_PER_UNIT);
        ok)
    {
        pricePerMeterColumn = columnId;
    }

    int defaultGroupingColumn = noColumn_;
    for (int i = 0; i < columnCount(); ++i)
    {
        if (i == pricePerMeterColumn ||
            ColumnType::STRING != getColumnFormat(i))
        {
            continue;
        }

        defaultGroupingColumn = i;
        break;
    }

    return defaultGroupingColumn;
}
