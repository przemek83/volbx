#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <memory>
#include <vector>

#include <QAbstractTableModel>

#include "Common/Formats.h"
#include "Common/SpecialColumns.h"

#include "Dataset.h"

class Dataset;
class DatasetDefinition;

/**
 * @brief 2d data model.
 */
class TableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit TableModel(std::unique_ptr<Dataset> dataset, QObject* parent = nullptr);

    ~TableModel() override = default;

    TableModel& operator=(const TableModel& other) = delete;
    TableModel(const TableModel& other) = delete;

    TableModel& operator=(TableModel&& other) = delete;
    TableModel(TableModel&& other) = delete;

    /**
     * @brief Overridem method for row count check.
     * @return row count.
     */
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    /**
     * @brief Override method for column count check.
     * @return column count.
     */
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    /**
     * @brief Overridem method returning data from model for given index.
     * @param index index used to retrieve data.
     * @return data.
     */
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    /**
     * @brief Overridem method returning data used for column header.
     * @param section column.
     * @param orientation orientation (vertical or horizontal).
     * @param role role of returned data.
     * @return data.
     */
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    /**
     * @brief fill max and min for given numeric column.
     * @param column column.
     * @param min minimum for column.
     * @param max maksimum cor column.
     */
    void getNumericRange(int column, double& min, double& max) const;

    /**
     * @brief Fill max and min for given date column.
     * @param column Column number.
     * @returns minimum date for column,
     *          maksimum date for column,
     *          existance of empty dates in column.
     */
    std::tuple<QDate, QDate, bool> getDateRange(int column) const;

    /**
     * @brief set possible string values for coulmn.
     * @param column column.
     * @param listToFill list of strings.
     */
    void getStringList(int column, QStringList& listToFill) const;

    /**
     * @brief get type of given column.
     * @return data format of given column.
     */
    DataFormat getColumnFormat(int column) const;

    /**
     * @brief get special column if exist (data, units, price).
     * @param columnTag searched column tag.
     * @return true if found (column number in int passed as ref.).
     */
    std::tuple<bool, int> getSpecialColumnIfExists(SpecialColumn columnTag) const;

    /**
     * @brief get dataset used in model.
     * @return dataset definition pointer.
     */
    const DatasetDefinition* getDatasetDefinition() const;

    bool isSpecialColumnsSet() const;

    bool isEmptyCellsDetected() const;

    int getDefaultGroupingColumn() const;

private:
    std::unique_ptr<Dataset> dataset_;

    static const int noColumn_;
};

#endif // TABLEMODEL_H
