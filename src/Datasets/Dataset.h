#pragma once

#include <memory>

#include <eible/ColumnType.h>
#include <QMap>
#include <QObject>
#include <QVariant>
#include <QVector>

#include <ColumnTag.h>

class DatasetDefinition;
class QDomDocument;
class QDomElement;

using Column = int;

class Dataset : public QObject
{
    Q_OBJECT
public:
    explicit Dataset(QString name, QObject* parent = nullptr);

    unsigned int rowCount() const;

    unsigned int columnCount() const;

    virtual inline QVariant* getData(int row, Column column)
    {
        if (getColumnFormat(column) == ColumnType::STRING)
        {
            if (data_[row][column].isNull())
                return &nullStringVariant_;
            if (data_[row][column].typeId() != QMetaType::QString)
                return &sharedStrings_[data_[row][column].toInt()];
        }
        return &data_[row][column];
    }

    ColumnType getColumnFormat(Column column) const;

    std::tuple<double, double> getNumericRange(Column column) const;

    /// @brief Get dates range got given column.
    /// @param column Column index.
    /// @return Minimum, maximum and flag if there are empty dates for column.
    std::tuple<QDate, QDate, bool> getDateRange(Column column) const;

    QStringList getStringList(Column column) const;

    /// @brief Get index of tagged column if available.
    /// @param columnTag Type of tagged column.
    /// @return Flag indicating there is tagged column and column index.
    std::tuple<bool, Column> getTaggedColumn(ColumnTag columnTag) const;

    QString getHeaderName(Column column) const;

    bool isValid() const;

    QString getName() const;

    bool initialize();

    bool loadData();

    QByteArray definitionToXml(unsigned int rowCount) const;

    QVector<QVector<QVariant>> retrieveSampleData();

    void setActiveColumns(const QVector<bool>& activeColumns);

    void setTaggedColumn(ColumnTag columnTag, Column column);

    QString getLastError() const;

protected:
    virtual bool analyze() = 0;

    virtual std::tuple<bool, QVector<QVector<QVariant>>> getSample() = 0;

    virtual std::tuple<bool, QVector<QVector<QVariant>>> getAllData() = 0;

    virtual void closeZip() = 0;

    void updateSampleDataStrings(QVector<QVector<QVariant>>& data) const;

    QVector<QVariant> sharedStrings_;

    bool valid_{false};

    static constexpr unsigned int SAMPLE_SIZE{10};

    QVector<ColumnType> columnTypes_;

    QStringList headerColumnNames_;

    QVector<bool> activeColumns_;

    unsigned int rowsCount_{0};

    unsigned int columnsCount_{0};

    QString error_;

    const QString XML_NAME{QStringLiteral("DATASET")};
    const QString XML_COLUMNS{QStringLiteral("COLUMNS")};
    const QString XML_COLUMN{QStringLiteral("COLUMN")};
    const QString XML_COLUMN_NAME{QStringLiteral("NAME")};
    const QString XML_COLUMN_FORMAT{QStringLiteral("FORMAT")};
    const QString XML_COLUMN_TAG{QStringLiteral("TAG")};
    const QString XML_COLUMN_TAG_DEPRECATED{QStringLiteral("SPECIAL_TAG")};
    const QString XML_ROW_COUNT{QStringLiteral("ROW_COUNT")};

private:
    void rebuildDefinitonUsingActiveColumnsOnly();

    bool isColumnTagged(ColumnTag tag) const;

    QDomElement columnsToXml(QDomDocument& xmlDocument) const;

    QDomElement rowCountToXml(QDomDocument& xmlDocument,
                              unsigned int rowCount) const;

    QVariant nullStringVariant_;

    const QString name_;

    QVector<QVector<QVariant>> sampleData_;

    /// Data of dataset. String columns got names in sharedStrings_.
    QVector<QVector<QVariant>> data_;

    /// Stores information about columns which are tagged.
    QMap<ColumnTag, Column> taggedColumns_;

Q_SIGNALS:
    void loadingPercentChanged(unsigned int newPercentage);
};
