#ifndef DATASET_H
#define DATASET_H

#include <memory>

#include <ColumnType.h>
#include <QStringList>
#include <QVariant>
#include <QVector>

#include "Common/SpecialColumns.h"
#include "DatasetDefinition.h"

class DatasetDefinition;

/**
 * @brief Representation of dataset.
 * Dataset definition is "formula" for creation of dataset and is inner
 * part of dataset.
 */
class Dataset
{
public:
    explicit Dataset(DatasetDefinition* definition);

    virtual ~Dataset();

    Dataset& operator=(const Dataset& other) = delete;
    Dataset(const Dataset& other) = delete;

    Dataset& operator=(Dataset&& other) = delete;
    Dataset(Dataset&& other) = delete;

    int rowCount() const;

    int columnCount() const;

    virtual inline QVariant* getData(int row, int column)
    {
        if (ColumnType::STRING == datasetDefinition_->getColumnFormat(column))
        {
            if (data_[row][column].isNull())
                return &nullStringVariant_;
            return &sharedStrings_[data_[row][column].toULongLong()];
        }
        else
        {
            return &data_[row][column];
        }
    }

    ColumnType getColumnFormat(int column) const;

    std::tuple<double, double> getNumericRange(int column) const;

    std::tuple<QDate, QDate, bool> getDateRange(int column) const;

    QStringList getStringList(int column) const;

    std::tuple<bool, int> getSpecialColumnIfExists(
        SpecialColumn columnTag) const;

    QString getHeaderName(int column) const;

    bool isValid() const;

    const DatasetDefinition* getDatasetDefinition() const;

    QString getName() const;

    virtual void init() = 0;

    QString getNameForTabBar();

protected:
    /// Definition done to create dataset. Memory owned by dataset object.
    DatasetDefinition* datasetDefinition_{nullptr};

    /// Data of dataset. String columns got indexes of strings in
    /// sharedStrings_.
    QVector<QVector<QVariant> > data_;

    /// Array with shared strings, Done for memory optimization.
    std::unique_ptr<QVariant[]> sharedStrings_{nullptr};

    /// Dataset is valid.
    bool valid_{false};

private:
    QVariant nullStringVariant_;
};

#endif  // DATASET_H
