#ifndef DATASETDEFINITION_H
#define DATASETDEFINITION_H

#include <QString>
#include <QVector>
#include <QStringList>
#include <QMap>
#include <QVariant>

#include "Common/Formats.h"
#include "Common/SpecialColumns.h"

/**
 * @brief Dataset definition is "formula" for creation of dataset.
 */
class DatasetDefinition
{
public:
    explicit DatasetDefinition(QString name);

    virtual ~DatasetDefinition();

    virtual bool isValid() const = 0;

    int rowCount() const;

    int columnCount() const;

    int getActiveColumnCount() const;

    inline DataFormat getColumnFormat(int column) const
    {
        Q_ASSERT(column >= 0 && column < columnCount());

        return columnsFormat_[column];
    }

    bool getSpecialColumnIfExists(SpecialColumn columnTag, int& column) const;

    QString getColumnName(int column) const;

    QString getName() const;

    const QString getError() const;

    ///Clear sample data when it is not necessary.
    void clearSampleData();

    const QVector<QVector<QVariant> >* getSampleData() const;

    void setActiveColumns(const QVector<bool> &activeColumns);

    const QVector<bool>* getActiveColumns() const;

    void setSpecialColumn(SpecialColumn columnTag, int column);

    QString dumpDatasetDefinition() const;

    virtual QVariant* getSharedStringTable() = 0;

protected:
    virtual void updateSampleDataStrings() = 0;

    bool valid_;

    bool isSpecialColumnTagged(SpecialColumn column) const;

    int rowsCount_;

    int columnsCount_;

    QVector<DataFormat> columnsFormat_;

    QStringList headerColumnNames_;

    ///Stores information about columns which are tagged as "special".
    QMap<SpecialColumn, int> specialColumns_;

    ///Dataset name.
    const QString name_;

    QString error_;

    QVector<QVector<QVariant> > sampleData_;

    ///Active columns information.
    QVector<bool> activeColumns_;

    ///If empty cell is encountered insert defined string.
    const QString emptyColName_;

    void rebuildDefinitonUsingActiveColumnsOnly();

    ///Number of lines in sample data.
    static const int sampleSize_;

    QVariant getDefaultVariantForFormat(const DataFormat format) const;

private:
    Q_DISABLE_COPY(DatasetDefinition)
};

#endif // DATASETDEFINITION_H
