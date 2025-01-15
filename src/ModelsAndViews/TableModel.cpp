#include "TableModel.h"

#include "Constants.h"

TableModel::TableModel(std::unique_ptr<Dataset> dataset, QObject* parent)
    : QAbstractTableModel(parent), dataset_(std::move(dataset))
{
}

TableModel::TableModel(std::unique_ptr<Dataset> dataset)
    : TableModel(std::move(dataset), nullptr)
{
}

int TableModel::rowCount([[maybe_unused]] const QModelIndex& parent) const
{
    return static_cast<int>(dataset_->rowCount());
}

int TableModel::columnCount([[maybe_unused]] const QModelIndex& parent) const
{
    return static_cast<int>(dataset_->columnCount());
}

QVariant TableModel::data(const QModelIndex& index, int role) const
{
    if (role == Qt::DisplayRole)
        return *dataset_->getData(index.row(), index.column());
    return {};
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation,
                                int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
        return dataset_->getHeaderName(section);
    return {};
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

std::tuple<bool, int> TableModel::getTaggedColumnIfExists(
    ColumnTag columnTag) const
{
    return dataset_->getTaggedColumn(columnTag);
}

QByteArray TableModel::definitionToXml(unsigned int rowCount) const
{
    return dataset_->definitionToXml(rowCount);
}

bool TableModel::areTaggedColumnsSet() const
{
    const auto [transDateColumnSet,
                transDateColumnId]{getTaggedColumnIfExists(ColumnTag::DATE)};
    const auto [priceColumnSet,
                priceColumnId]{getTaggedColumnIfExists(ColumnTag::VALUE)};

    return transDateColumnSet && priceColumnSet;
}

int TableModel::getDefaultGroupingColumn() const
{
    for (int column{0}; column < columnCount(); ++column)
        if (getColumnFormat(column) == ColumnType::STRING)
            return column;
    return Constants::NOT_SET_COLUMN;
}
