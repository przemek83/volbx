#ifndef DATASETDEFINITION_H
#define DATASETDEFINITION_H

#include <memory>

#include <ColumnType.h>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QVector>

#include "Common/SpecialColumns.h"

/**
 * @brief Dataset definition is "formula" for creation of dataset.
 */
class DatasetDefinition
{
public:
    explicit DatasetDefinition(QString name);

    virtual ~DatasetDefinition() = default;

    DatasetDefinition& operator=(const DatasetDefinition& other) = delete;
    DatasetDefinition(const DatasetDefinition& other) = delete;

    DatasetDefinition& operator=(DatasetDefinition&& other) = delete;
    DatasetDefinition(DatasetDefinition&& other) = delete;

    virtual bool isValid() const;

    int rowCount() const;

    int columnCount() const;

    int getActiveColumnCount() const;

    inline ColumnType getColumnFormat(int column) const
    {
        Q_ASSERT(column >= 0 && column < columnCount());

        return columnTypes_[column];
    }

    std::tuple<bool, int> getSpecialColumnIfExists(
        SpecialColumn columnTag) const;

    QString getColumnName(int column) const;

    QString getName() const;

    QString getError() const;

    /// Clear sample data when it is not necessary.
    void clearSampleData();

    const QVector<QVector<QVariant> >* getSampleData() const;

    void setActiveColumns(const QVector<bool>& activeColumns);

    const QVector<bool>* getActiveColumns() const;

    void setSpecialColumn(SpecialColumn columnTag, int column);

    QString dumpDatasetDefinition() const;

    virtual std::unique_ptr<QVariant[]> getSharedStringTable() = 0;

    /// Definition to bytes array.
    void toXml(QByteArray& data, int rowCountNumber) const;

protected:
    int rowsCount_{0};

    int columnsCount_{0};

    QVector<ColumnType> columnTypes_;

    QStringList headerColumnNames_;

    /// Stores information about columns which are tagged as "special".
    QMap<SpecialColumn, int> specialColumns_;

    /// Dataset name.
    const QString name_;

    QString error_;

    QVector<QVector<QVariant> > sampleData_;

    /// Active columns information.
    QVector<bool> activeColumns_;

    /// If empty cell is encountered insert defined string.
    const QString emptyColName_;

    void rebuildDefinitonUsingActiveColumnsOnly();

    /// Number of lines in sample data.
    static constexpr int SAMPLE_SIZE{10};

    bool isSpecialColumnTagged(SpecialColumn column) const;

    QVariant getDefaultVariantForFormat(const ColumnType format) const;

    /// Names used in definition file.
    const QString DATASET_NAME{QStringLiteral("DATASET")};
    const QString DATASET_COLUMNS{QStringLiteral("COLUMNS")};
    const QString DATASET_COLUMN{QStringLiteral("COLUMN")};
    const QString DATASET_COLUMN_NAME{QStringLiteral("NAME")};
    const QString DATASET_COLUMN_FORMAT{QStringLiteral("FORMAT")};
    const QString DATASET_COLUMN_SPECIAL_TAG{QStringLiteral("SPECIAL_TAG")};
    const QString DATASET_ROW_COUNT{QStringLiteral("ROW_COUNT")};

    bool valid_{false};
};

#endif  // DATASETDEFINITION_H
