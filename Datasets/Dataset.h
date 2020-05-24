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
    explicit Dataset(const QString& name, QObject* parent = nullptr);

    virtual ~Dataset();

    unsigned int rowCount() const;

    unsigned int columnCount() const;

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

    ColumnType getColumnFormat(unsigned int column) const;

    std::tuple<double, double> getNumericRange(unsigned int column) const;

    std::tuple<QDate, QDate, bool> getDateRange(unsigned int column) const;
    QStringList getStringList(unsigned int column) const;

    std::tuple<bool, unsigned int> getSpecialColumnIfExists(
        SpecialColumn columnTag) const;

    QString getHeaderName(unsigned int column) const;

    bool isValid() const;

    QString getName() const;

    bool initialize();

    bool loadData();

    QString getNameForTabBar();

    QByteArray definitionToXml(unsigned int rowCount) const;

    QVector<QVector<QVariant>> retrieveSampleData() const;

    void setActiveColumns(const QVector<bool>& activeColumns);

    void setSpecialColumn(SpecialColumn columnTag, unsigned int column);

    QString getError() const;

    QString dumpDatasetDefinition() const;

    unsigned int getActiveColumnCount() const;

    void updateSampleDataStrings(QVector<QVector<QVariant>>& data) const;

protected:
    virtual bool analyze() = 0;

    virtual std::tuple<bool, QVector<QVector<QVariant>>> getSample() = 0;

    virtual std::tuple<bool, QVector<QVector<QVariant>>> getAllData() = 0;

    QVector<QVariant> sharedStrings_;

    /// Dataset is valid.
    bool valid_{false};

    /// Number of lines in sample data.
    static constexpr unsigned int SAMPLE_SIZE{10};

    QVector<ColumnType> columnTypes_;

    QStringList headerColumnNames_;

    /// Active columns information.
    QVector<bool> activeColumns_;

    unsigned int rowsCount_{0};

    unsigned int columnsCount_{0};

    QString error_;

    /// Names used in definition file.
    const QString DATASET_NAME{QStringLiteral("DATASET")};
    const QString DATASET_COLUMNS{QStringLiteral("COLUMNS")};
    const QString DATASET_COLUMN{QStringLiteral("COLUMN")};
    const QString DATASET_COLUMN_NAME{QStringLiteral("NAME")};
    const QString DATASET_COLUMN_FORMAT{QStringLiteral("FORMAT")};
    const QString DATASET_COLUMN_SPECIAL_TAG{QStringLiteral("SPECIAL_TAG")};
    const QString DATASET_ROW_COUNT{QStringLiteral("ROW_COUNT")};

private:
    void rebuildDefinitonUsingActiveColumnsOnly();

    bool isSpecialColumnTagged(SpecialColumn column) const;

    QVariant nullStringVariant_;

    const QString name_;

    QVector<QVector<QVariant>> sampleData_;

    /// Data of dataset. String columns got names in sharedStrings_.
    QVector<QVector<QVariant>> data_;

    /// Stores information about columns which are tagged as "special".
    QMap<SpecialColumn, unsigned int> specialColumns_;

signals:
    void loadingPercentChanged(unsigned int newPercentage);
};

#endif  // DATASET_H
