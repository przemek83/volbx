#ifndef DATASET_H
#define DATASET_H

#include <memory>

#include <ColumnType.h>
#include <QMap>
#include <QObject>
#include <QVariant>
#include <QVector>

#include "Common/SpecialColumns.h"

class DatasetDefinition;

/**
 * @brief Representation of dataset.
 * Dataset definition is "formula" for creation of dataset and is inner
 * part of dataset.
 */
class Dataset : public QObject
{
    Q_OBJECT
public:
    Dataset(const QString& name, QObject* parent = nullptr);

    virtual ~Dataset();

    int rowCount() const;

    int columnCount() const;

    virtual inline QVariant* getData(int row, int column)
    {
        if (ColumnType::STRING == getColumnFormat(column))
        {
            if (data_[row][column].isNull())
                return &nullStringVariant_;
            if (data_[row][column].type() != QVariant::String)
                return &sharedStrings_[data_[row][column].toULongLong()];
        }
        return &data_[row][column];
    }

    ColumnType getColumnFormat(int column) const;

    std::tuple<double, double> getNumericRange(int column) const;

    std::tuple<QDate, QDate, bool> getDateRange(int column) const;
    QStringList getStringList(int column) const;

    std::tuple<bool, int> getSpecialColumnIfExists(
        SpecialColumn columnTag) const;

    QString getHeaderName(int column) const;

    bool isValid() const;

    QString getName() const;

    virtual bool analyze() = 0;

    virtual bool loadData() = 0;

    QString getNameForTabBar();

    QByteArray definitionToXml(int rowCount) const;

    const QVector<QVector<QVariant> >* getSampleData() const;

    /// Clear sample data when it is not necessary.
    void clearSampleData();

    void setActiveColumns(const QVector<bool>& activeColumns);

    void setSpecialColumn(SpecialColumn columnTag, int column);

    QString getError() const;

    QString dumpDatasetDefinition() const;

    int getActiveColumnCount() const;

protected:
    void rebuildDefinitonUsingActiveColumnsOnly();

    virtual std::unique_ptr<QVariant[]> getSharedStringTable() = 0;

    /// Data of dataset. String columns got names in sharedStrings_.
    QVector<QVector<QVariant> > data_;

    /// Array with shared strings, Done for memory optimization.
    std::unique_ptr<QVariant[]> sharedStrings_{nullptr};

    /// Dataset is valid.
    bool valid_{false};

    /// Number of lines in sample data.
    static constexpr int SAMPLE_SIZE{10};

    QVector<ColumnType> columnTypes_;

    QStringList headerColumnNames_;

    /// Active columns information.
    QVector<bool> activeColumns_;

    QVector<QVector<QVariant> > sampleData_;

    int rowsCount_{0};

    int columnsCount_{0};

    QString error_;

    /// Names used in definition file.
    const QString DATASET_NAME{QStringLiteral("DATASET")};
    const QString DATASET_COLUMNS{QStringLiteral("COLUMNS")};
    const QString DATASET_COLUMN{QStringLiteral("COLUMN")};
    const QString DATASET_COLUMN_NAME{QStringLiteral("NAME")};
    const QString DATASET_COLUMN_FORMAT{QStringLiteral("FORMAT")};
    const QString DATASET_COLUMN_SPECIAL_TAG{QStringLiteral("SPECIAL_TAG")};
    const QString DATASET_ROW_COUNT{QStringLiteral("ROW_COUNT")};

    /// Stores information about columns which are tagged as "special".
    QMap<SpecialColumn, int> specialColumns_;

private:
    bool isSpecialColumnTagged(SpecialColumn column) const;

    QVariant nullStringVariant_;

    /// Dataset name.
    const QString name_;

signals:
    void loadingPercentChanged(unsigned int newPercentage);
};

#endif  // DATASET_H
