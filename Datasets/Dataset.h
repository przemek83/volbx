#pragma once

#include <memory>

#include <ColumnType.h>
#include <QMap>
#include <QObject>
#include <QVariant>
#include <QVector>

#include <ColumnTag.h>

class DatasetDefinition;
class QDomDocument;
class QDomElement;

/**
 * @class Dataset
 * @brief Representation for set of data.
 */
class Dataset : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Dataset constructor
     * @param name Dataset name.
     * @param parent Parent object.
     */
    explicit Dataset(const QString& name, QObject* parent = nullptr);

    virtual ~Dataset();

    /**
     * @brief Get number of rows in dataset.
     * @return Number of rows.
     */
    unsigned int rowCount() const;

    /**
     * @brief Get number of columns in dataset.
     * @return Number of columns.
     */
    unsigned int columnCount() const;

    /**
     * @brief Get data QVariant for given row and column.
     * @param row Row for which data need to be retrieved.
     * @param column Column for which data need to be retrieved.
     * @return Pointer to QVariant with data.
     */
    virtual inline QVariant* getData(int row, int column)
    {
        if (getColumnFormat(column) == ColumnType::STRING)
        {
            if (data_[row][column].isNull())
                return &nullStringVariant_;
            if (data_[row][column].type() != QVariant::String)
                return &sharedStrings_[data_[row][column].toULongLong()];
        }
        return &data_[row][column];
    }

    /**
     * @brief Get format of column with given index.
     * @param column Column index.
     * @return Type of column.
     */
    ColumnType getColumnFormat(unsigned int column) const;

    /**
     * @brief Get numeric range for given column.
     * @param column Column index.
     * @return Minimum and maximum in given column.
     */
    std::tuple<double, double> getNumericRange(unsigned int column) const;

    /**
     * @brief Get dates range got given column.
     * @param column Column index.
     * @return Minimum, maximum and flag if there are empty dates for column.
     */
    std::tuple<QDate, QDate, bool> getDateRange(unsigned int column) const;

    /**
     * @brief Get list of unique strings in given column.
     * @param column Column index.
     * @return String list for given column.
     */
    QStringList getStringList(unsigned int column) const;

    /**
     * @brief Get index of tagged column if available.
     * @param columnTag Type of tagged column.
     * @return Flag indicating there is tagged column and column index.
     */
    std::tuple<bool, unsigned int> getTaggedColumn(ColumnTag columnTag) const;

    /**
     * @brief Get header name for given column.
     * @param column Column index.
     * @return Column name.
     */
    QString getHeaderName(unsigned int column) const;

    /**
     * @brief Check if dataset is valid.
     * @return True if valid, false otherwise.
     */
    bool isValid() const;

    /**
     * @brief Get dataset name
     * @return Dataset name.
     */
    QString getName() const;

    /**
     * @brief Initialize dataset.
     * @return True if succeed, false otherwise.
     */
    bool initialize();

    /**
     * @brief Load data into dataset.
     * @return True if succeed, false otherwise.
     */
    bool loadData();

    /**
     * @brief Create XML with definition of dataset
     * @param rowCount Number of rows active in view.
     * @return Definition as QByteArray.
     */
    QByteArray definitionToXml(unsigned int rowCount) const;

    /**
     * @brief Retrieve sample data (data is moved).
     * @return Sample data.
     */
    QVector<QVector<QVariant>> retrieveSampleData() const;

    /**
     * @brief Set active columns in dataset.
     * @param activeColumns Active columns flags vector.
     */
    void setActiveColumns(const QVector<bool>& activeColumns);

    /**
     * @brief Set tagged column in dataset.
     * @param columnTag Column tag.
     * @param column Column index.
     */
    void setTaggedColumn(ColumnTag columnTag, unsigned int column);

    /**
     * @brief Get last error.
     * @return Last error.
     */
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
    const QString XML_COLUMN_TAG{QStringLiteral("SPECIAL_TAG")};
    const QString XML_ROW_COUNT{QStringLiteral("ROW_COUNT")};

private:
    void rebuildDefinitonUsingActiveColumnsOnly();

    bool isColumnTagged(ColumnTag column) const;

    QDomElement columnsToXml(QDomDocument& xmlDocument) const;

    QDomElement rowCountToXml(QDomDocument& xmlDocument,
                              unsigned int rowCount) const;

    QVariant nullStringVariant_;

    const QString name_;

    QVector<QVector<QVariant>> sampleData_;

    /// Data of dataset. String columns got names in sharedStrings_.
    QVector<QVector<QVariant>> data_;

    /// Stores information about columns which are tagged.
    QMap<ColumnTag, unsigned int> taggedColumns_;

signals:
    /**
     * @brief Signal for loading percent changes
     * @param newPercentage New percent.
     */
    void loadingPercentChanged(unsigned int newPercentage);
};
