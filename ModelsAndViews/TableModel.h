#pragma once

#include <memory>

#include <ColumnType.h>
#include <QAbstractTableModel>

#include "Dataset.h"

/**
 * @brief 2d data model.
 */
class TableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit TableModel(std::unique_ptr<Dataset> dataset,
                        QObject* parent = nullptr);

    ~TableModel() override = default;

    /**
     * @brief Overridden method for row count check.
     * @return row count.
     */
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    /**
     * @brief Override method for column count check.
     * @return column count.
     */
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    /**
     * @brief Overridden method returning data from model for given index.
     * @param index index used to retrieve data.
     * @return data.
     */
    QVariant data(const QModelIndex& index,
                  int role = Qt::DisplayRole) const override;

    /**
     * @brief Overridden method returning data used for column header.
     * @param section column.
     * @param orientation orientation (vertical or horizontal).
     * @param role role of returned data.
     * @return data.
     */
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    /**
     * @brief fill max and min for given numeric column.
     * @param column Column number.
     * @returns Minimum for column,
     *          maximum cor column.
     */
    std::tuple<double, double> getNumericRange(int column) const;

    /**
     * @brief Fill max and min for given date column.
     * @param column Column number.
     * @returns minimum date for column,
     *          maximum date for column,
     *          existence of empty dates in column.
     */
    std::tuple<QDate, QDate, bool> getDateRange(int column) const;

    /**
     * @brief set possible string values for column.
     * @param column Column number.
     * @return List of strings.
     */
    QStringList getStringList(int column) const;

    /**
     * @brief get type of given column.
     * @return data format of given column.
     */
    ColumnType getColumnFormat(int column) const;

    /**
     * @brief get tagged column if exist (date, value).
     * @param columnTag searched column tag.
     * @return true if found (column number in int passed as ref.).
     */
    std::tuple<bool, int> getTaggedColumnIfExists(ColumnTag columnTag) const;

    /**
     * @brief get dataset used in model.
     * @return dataset definition pointer.
     */
    QByteArray definitionToXml(int rowCount) const;

    bool areTaggedColumnsSet() const;

    int getDefaultGroupingColumn() const;

private:
    std::unique_ptr<Dataset> dataset_;
};
