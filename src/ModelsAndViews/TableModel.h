#pragma once

#include <memory>

#include <eible/ColumnType.h>
#include <QAbstractTableModel>

#include "Dataset.h"

class TableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit TableModel(std::unique_ptr<Dataset> dataset);

    int rowCount(const QModelIndex& parent) const override;

    int columnCount(const QModelIndex& parent) const override;

    QVariant data(const QModelIndex& index, int role) const override;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role) const override;

    std::tuple<double, double> getNumericRange(int column) const;

    /// @brief Fill max and min for given date column.
    /// @param column Column number.
    /// @returns minimum date for column, maximum date for column, existence of
    /// empty dates in column.
    std::tuple<QDate, QDate, bool> getDateRange(int column) const;

    QStringList getStringList(int column) const;

    ColumnType getColumnFormat(int column) const;

    std::tuple<bool, int> getTaggedColumnIfExists(ColumnTag columnTag) const;

    QByteArray definitionToXml(int rowCount) const;

    bool areTaggedColumnsSet() const;

    int getDefaultGroupingColumn() const;

private:
    std::unique_ptr<Dataset> dataset_;
};
