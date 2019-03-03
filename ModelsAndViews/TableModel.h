#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <vector>

#include <QAbstractTableModel>

#include "Common/Formats.h"
#include "Common/SpecialColumns.h"

class Dataset;
class DatasetDefinition;

/**
 * @brief 2d data model.
 */
class TableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    TableModel(Dataset* dataset, QObject* parent = nullptr);

    ~TableModel() override;

    /**
     * @brief Overridem method for row count check.
     * @param parent index of parent.
     * @return row count.
     */
    int rowCount(const QModelIndex& = QModelIndex()) const override;

    /**
     * @brief Overridem method for column count check.
     * @param parent index of parent.
     * @return column count.
     */
    int columnCount(const QModelIndex& = QModelIndex()) const override;

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
     * @brief fill max and min for given date column.
     * @param column column.
     * @param min minimum date for column.
     * @param max maksimum date for column.
     */
    void getDateRange(int column, QDate& min, QDate& max, bool& emptyDates) const;

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
     * @param column column number to fill if found.
     * @return true if found (column number in int passed as ref.).
     */
    bool getSpecialColumnIfExists(SpecialColumn columnTag, int& column) const;

    /**
     * @brief get dataset used in model.
     * @return dataset definition pointer.
     */
    const DatasetDefinition* getDatasetDefinition() const;

    bool isSpecialColumnsSet() const;

    bool isEmptyCellsDetected() const;

    int getDefaultGroupingColumn() const;

private:
    Q_DISABLE_COPY(TableModel)

    ///Dataset.
    Dataset* dataset_;

    static const int noColumn_;
};

#endif // TABLEMODEL_H
