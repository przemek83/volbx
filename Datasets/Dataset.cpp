#include "Dataset.h"

#include <QDate>
#include <QDomDocument>

#include <Constants.h>

Dataset::Dataset(QString name, QObject* parent)
    : QObject(parent),
      nullStringVariant_(QVariant::String),
      name_(std::move(name))
{
}

unsigned int Dataset::rowCount() const { return rowsCount_; }

unsigned int Dataset::columnCount() const { return columnsCount_; }

ColumnType Dataset::getColumnFormat(int column) const
{
    Q_ASSERT(column >= 0 && column < static_cast<int>(columnCount()));
    return columnTypes_[column];
}

std::tuple<double, double> Dataset::getNumericRange(int column) const
{
    Q_ASSERT(ColumnType::NUMBER == getColumnFormat(column));
    double min{0.};
    double max{0.};
    bool first{true};
    for (int i = 0; i < static_cast<int>(rowCount()); ++i)
    {
        const double value{data_[i][column].toDouble()};
        if (first)
        {
            min = value;
            max = value;
            first = false;
            continue;
        }

        if (value < min)
            min = value;

        if (value > max)
            max = value;
    }
    return {min, max};
}

std::tuple<QDate, QDate, bool> Dataset::getDateRange(int column) const
{
    Q_ASSERT(ColumnType::DATE == getColumnFormat(column));
    QDate minDate;
    QDate maxDate;
    bool emptyDates{false};
    bool first{true};
    for (int i = 0; i < static_cast<int>(rowCount()); ++i)
    {
        const QVariant& dateVariant{data_[i][column]};
        if (dateVariant.isNull())
        {
            emptyDates = true;
            continue;
        }
        QDate date{dateVariant.toDate()};
        if (first)
        {
            minDate = date;
            maxDate = date;
            first = false;
            continue;
        }

        if (date < minDate)
            minDate = date;

        if (date > maxDate)
            maxDate = date;
    }

    return {minDate, maxDate, emptyDates};
}

QStringList Dataset::getStringList(unsigned int column) const
{
    Q_ASSERT(ColumnType::STRING == getColumnFormat(column));
    QStringList listToFill;
    listToFill.reserve(static_cast<int>(rowCount()));
    for (const auto& row : data_)
    {
        if (row[column].isNull())
            continue;

        if (row[column].type() == QVariant::String)
        {
            listToFill.append(row[column].toString());
            continue;
        }
        const int index{row[column].toInt()};
        listToFill.append(sharedStrings_[index].toString());
    }
    listToFill.removeDuplicates();
    return listToFill;
}

std::tuple<bool, unsigned int> Dataset::getTaggedColumn(
    ColumnTag columnTag) const
{
    if (isColumnTagged(columnTag))
        return {true, taggedColumns_[columnTag]};
    return {false, Constants::NOT_SET_COLUMN};
}

QString Dataset::getHeaderName(int column) const
{
    if (static_cast<int>(columnsCount_) >= column + 1)
        return headerColumnNames_[column];
    Q_ASSERT(false);
    return QLatin1String("");
}

bool Dataset::isValid() const { return valid_; }

QString Dataset::getName() const { return name_; }

bool Dataset::initialize()
{
    if (!analyze())
        return false;
    bool success{false};
    std::tie(success, sampleData_) = getSample();
    return success;
}

bool Dataset::loadData()
{
    bool success{false};
    std::tie(success, data_) = getAllData();
    rebuildDefinitonUsingActiveColumnsOnly();
    closeZip();
    return success;
}

QDomElement Dataset::columnsToXml(QDomDocument& xmlDocument) const
{
    QDomElement columns{xmlDocument.createElement(XML_COLUMNS)};
    for (int column = 0; column < static_cast<int>(columnsCount_); ++column)
    {
        QDomElement node{xmlDocument.createElement(XML_COLUMN)};
        node.setAttribute(XML_COLUMN_NAME, headerColumnNames_.at(column));
        node.setAttribute(XML_COLUMN_FORMAT,
                          static_cast<int>(columnTypes_.at(column)));
        QMapIterator<ColumnTag, int> it(taggedColumns_);
        if (it.findNext(column))
            node.setAttribute(XML_COLUMN_TAG,
                              QString::number(static_cast<int>(it.key())));
        columns.appendChild(node);
    }
    return columns;
}

QDomElement Dataset::rowCountToXml(QDomDocument& xmlDocument,
                                   unsigned int rowCount) const
{
    QDomElement rowCountElement{xmlDocument.createElement(XML_ROW_COUNT)};
    rowCountElement.setAttribute(XML_ROW_COUNT, QString::number(rowCount));
    return rowCountElement;
}

QByteArray Dataset::definitionToXml(unsigned int rowCount) const
{
    QDomDocument xmlDocument;
    QDomElement root{xmlDocument.createElement(XML_NAME)};
    root.appendChild(columnsToXml(xmlDocument));
    root.appendChild(rowCountToXml(xmlDocument, rowCount));
    xmlDocument.appendChild(root);
    return xmlDocument.toByteArray();
}

QVector<QVector<QVariant>> Dataset::retrieveSampleData()
{
    return std::move(sampleData_);
}

void Dataset::setActiveColumns(const QVector<bool>& activeColumns)
{
    activeColumns_ = activeColumns;
}

void Dataset::setTaggedColumn(ColumnTag columnTag, int column)
{
    taggedColumns_[columnTag] = column;
}

QString Dataset::getLastError() const { return error_; }

void Dataset::rebuildDefinitonUsingActiveColumnsOnly()
{
    QVector<ColumnType> rebuiltColumnsFormat;
    QStringList rebuiltHeaderColumnNames;
    QMap<ColumnTag, int> rebuiltTaggedColumns;
    int activeColumnNumber{0};
    const ColumnTag dateTag{ColumnTag::DATE};
    const ColumnTag priceTag{ColumnTag::VALUE};
    const bool dateColumnTagged{isColumnTagged(dateTag)};
    const bool priceColumnTagged{isColumnTagged(priceTag)};

    for (int i = 0; i < activeColumns_.count(); ++i)
    {
        if (!activeColumns_.at(i))
            continue;

        rebuiltColumnsFormat.push_back(columnTypes_[i]);
        rebuiltHeaderColumnNames << headerColumnNames_[i];
        if (dateColumnTagged && taggedColumns_.value(dateTag) == i)
            rebuiltTaggedColumns[dateTag] = activeColumnNumber;
        if (priceColumnTagged && taggedColumns_.value(priceTag) == i)
            rebuiltTaggedColumns[priceTag] = activeColumnNumber;
        activeColumnNumber++;
    }

    columnTypes_ = rebuiltColumnsFormat;
    headerColumnNames_ = rebuiltHeaderColumnNames;
    taggedColumns_ = rebuiltTaggedColumns;
    columnsCount_ = static_cast<unsigned int>(activeColumns_.count(true));
    activeColumns_.clear();
}

void Dataset::updateSampleDataStrings(QVector<QVector<QVariant>>& data) const
{
    if (sharedStrings_.isEmpty())
        return;

    for (int i = 0; i < static_cast<int>(columnCount()); ++i)
    {
        if (columnTypes_.at(i) != ColumnType::STRING)
            continue;

        for (auto& sampleDataRow : data)
        {
            if (sampleDataRow[i].type() != QVariant::Int)
                continue;

            const int index{sampleDataRow[i].toInt()};
            sampleDataRow[i] =
                (index > sharedStrings_.size() ? 0 : sharedStrings_[index]);
        }
    }
}

bool Dataset::isColumnTagged(ColumnTag column) const
{
    return taggedColumns_.contains(column);
}
