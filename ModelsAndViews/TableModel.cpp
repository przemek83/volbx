#include <QDebug>

#include "Datasets/Dataset.h"
#include "Common/Constants.h"

#include "TableModel.h"

const int TableModel::noColumn_ = -1;

TableModel::TableModel(Dataset* dataset, QObject* parent) :
    QAbstractTableModel(parent),
    dataset_(dataset)
{

}

TableModel::~TableModel()
{
    delete dataset_;
}

int TableModel::rowCount(const QModelIndex& /*parent*/) const
{
    return dataset_->rowCount();
}

int TableModel::columnCount(const QModelIndex& /*parent*/) const
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

void TableModel::getNumericRange(int column, double& min, double& max) const
{
    dataset_->getNumericRange(column, min, max);
}

void TableModel::getDateRange(int column,
                              QDate& min,
                              QDate& max,
                              bool& emptyDates) const
{
    dataset_->getDateRange(column, min, max, emptyDates);
}

void TableModel::getStringList(int column, QStringList& listToFill) const
{
    dataset_->getStringList(column, listToFill);
}

DataFormat TableModel::getColumnFormat(int column) const
{
    return dataset_->getColumnFormat(column);
}

bool TableModel::getSpecialColumnIfExists(SpecialColumn columnTag,
                                          int& column) const
{
    return dataset_->getSpecialColumnIfExists(columnTag, column);
}

const DatasetDefinition* TableModel::getDatasetDefinition() const
{
    return dataset_->getDatasetDefinition();
}

bool TableModel::isSpecialColumnsSet() const
{
    int transDateColumn = noColumn_;
    int priceColumn = noColumn_;
    if (!getSpecialColumnIfExists(SPECIAL_COLUMN_TRANSACTION_DATE, transDateColumn) ||
        !getSpecialColumnIfExists(SPECIAL_COLUMN_PRICE_PER_UNIT, priceColumn))
    {
        return false;
    }
    return true;
}

bool TableModel::isEmptyCellsDetected() const
{
    //TODO or delete.
    return true;
}

int TableModel::getDefaultGroupingColumn() const
{
    int pricePerMeterColumn = noColumn_;
    getSpecialColumnIfExists(SPECIAL_COLUMN_PRICE_PER_UNIT, pricePerMeterColumn);

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

