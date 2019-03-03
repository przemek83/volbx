#ifndef DATASET_H
#define DATASET_H

#include <QVariant>
#include <QVector>
#include <QStringList>

#include "Common/Formats.h"
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

    int rowCount() const;

    int columnCount() const;

    virtual inline QVariant* getData(int row, int column)
    {
        if (DATA_FORMAT_STRING == datasetDefinition_->getColumnFormat(column))
        {
            return &sharedStrings_[data_[row][column].toInt()];
        }
        else
        {
            return &data_[row][column];
        }
    }

    DataFormat getColumnFormat(int column) const;

    void getNumericRange(int column, double& min, double& max) const;

    void getDateRange(int column, QDate& min, QDate& max, bool& emptyDates) const;

    void getStringList(int column, QStringList& listToFill) const;

    bool getSpecialColumnIfExists(SpecialColumn columnTag, int& column) const;

    QString getHeaderName(int column) const;

    bool isValid() const;

    const DatasetDefinition* getDatasetDefinition() const;

    QString getName() const;

    virtual void init() = 0;

    QString getNameForTabBar();

protected:
    ///Definition done to create dataset. Memory owned by dataset object.
    DatasetDefinition* datasetDefinition_ {nullptr};

    ///Data of dataset. String columns got indexes of strings in sharedStrings_.
    QVector<QVector<QVariant> > data_;

    ///Dataset is valid.
    bool valid_ {false};

    ///Array with shared strings, Done for memory optimization.
    QVariant* sharedStrings_ {nullptr};

private:
    Q_DISABLE_COPY(Dataset)
};

#endif // DATASET_H
