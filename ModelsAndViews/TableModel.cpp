#include "TableModel.h"

#include <QDebug>

#include "Common/Constants.h"
#include "Datasets/Dataset.h"

const int TableModel::noColumn_ = Constants::NOT_SET_COLUMN;

TableModel::TableModel(std::unique_ptr<Dataset> dataset, QObject* parent) :
    QAbstractTableModel(parent), dataset_(std::move(dataset))
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

QVariant TableModel::headerData(int section,
                                Qt::Orientation orientation,
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

void TableModel::getStringList(int column, QStringList& listToFill) const
{
    dataset_->getStringList(column, listToFill);
}

DataFormat TableModel::getColumnFormat(int column) const
{
    return dataset_->getColumnFormat(column);
}

std::tuple<bool, int> TableModel::getSpecialColumnIfExists(SpecialColumn columnTag) const
{
    return dataset_->getSpecialColumnIfExists(columnTag);
}

const DatasetDefinition* TableModel::getDatasetDefinition() const
{
    return dataset_->getDatasetDefinition();
}

bool TableModel::isSpecialColumnsSet() const
{
    auto [transDateColumnSet, transDateColumnId] =
        getSpecialColumnIfExists(SPECIAL_COLUMN_TRANSACTION_DATE);
    auto [priceColumnSet, priceColumnId] =
        getSpecialColumnIfExists(SPECIAL_COLUMN_PRICE_PER_UNIT);

    return transDateColumnSet && priceColumnSet;
}

bool TableModel::isEmptyCellsDetected() const
{
    //TODO or delete.
    return true;
}

int TableModel::getDefaultGroupingColumn() const
{
    int pricePerMeterColumn = noColumn_;
    if (auto [ok, columnId] = getSpecialColumnIfExists(SPECIAL_COLUMN_PRICE_PER_UNIT); ok)
    {
        pricePerMeterColumn = columnId;
    }

    int defaultGroupingColumn = noColumn_;
    for (int i = 0; i < columnCount(); ++i)
    {
        if (i == pricePerMeterColumn || DATA_FORMAT_STRING != getColumnFormat(i))
        {
            continue;
        }

        defaultGroupingColumn = i;
        break;
    }

    return defaultGroupingColumn;
}
