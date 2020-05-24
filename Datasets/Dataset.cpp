#include "Dataset.h"

#include <QDate>
#include <QDomDocument>

#include "Common/Constants.h"

Dataset::Dataset(const QString& name, QObject* parent)
    : QObject(parent), nullStringVariant_(QVariant::String), name_(name)
{
}

Dataset::~Dataset() {}

unsigned int Dataset::rowCount() const { return rowsCount_; }

unsigned int Dataset::columnCount() const { return columnsCount_; }

ColumnType Dataset::getColumnFormat(unsigned int column) const
{
    Q_ASSERT(column >= 0 && column < columnCount());
    return columnTypes_[column];
}

std::tuple<double, double> Dataset::getNumericRange(unsigned int column) const
{
    Q_ASSERT(ColumnType::NUMBER == getColumnFormat(column));
    double min{0.};
    double max{0.};
    bool first{true};
    for (unsigned int i = 0; i < rowCount(); ++i)
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

std::tuple<QDate, QDate, bool> Dataset::getDateRange(unsigned int column) const
{
    Q_ASSERT(ColumnType::DATE == getColumnFormat(column));
    QDate minDate;
    QDate maxDate;
    bool emptyDates{false};
    bool first{true};
    for (unsigned int i = 0; i < rowCount(); ++i)
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
    listToFill.reserve(rowCount());
    for (const auto& row : data_)
    {
        if (row[column].isNull())
            continue;

        if (row[column].type() == QVariant::String)
        {
            listToFill.append(row[column].toString());
            continue;
        }
        const uint32_t index{row[column].toUInt()};
        listToFill.append(sharedStrings_[index].toString());
    }
    listToFill.removeDuplicates();
    return listToFill;
}

std::tuple<bool, unsigned int> Dataset::getSpecialColumn(
    SpecialColumn columnTag) const
{
    if (isSpecialColumnTagged(columnTag))
        return {true, specialColumns_[columnTag]};
    return {false, Constants::NOT_SET_COLUMN};
}

QString Dataset::getHeaderName(unsigned int column) const
{
    if (columnsCount_ - 1 >= column)
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
    return success;
}

QString Dataset::getNameForTabBar()
{
    QString tabName{getName()};
    auto [exist, column] = getSpecialColumn(SpecialColumn::PRICE_PER_UNIT);
    if (exist)
        tabName.append(" (" + getHeaderName(column) + ")");
    return tabName;
}

QDomElement Dataset::columnsToXml(QDomDocument& xmlDocument) const
{
    QDomElement columns{xmlDocument.createElement(XML_COLUMNS)};
    for (unsigned int column = 0; column < columnsCount_; ++column)
    {
        QDomElement node{xmlDocument.createElement(XML_COLUMN)};
        node.setAttribute(XML_COLUMN_NAME, headerColumnNames_.at(column));
        node.setAttribute(XML_COLUMN_FORMAT,
                          static_cast<int>(columnTypes_.at(column)));
        QMapIterator<SpecialColumn, unsigned int> it(specialColumns_);
        if (it.findNext(column))
            node.setAttribute(XML_COLUMN_SPECIAL_TAG,
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

QVector<QVector<QVariant>> Dataset::retrieveSampleData() const
{
    return std::move(sampleData_);
}

void Dataset::setActiveColumns(const QVector<bool>& activeColumns)
{
    activeColumns_ = activeColumns;
}

void Dataset::setSpecialColumn(SpecialColumn columnTag, unsigned int column)
{
    specialColumns_[columnTag] = column;
}

QString Dataset::getError() const { return error_; }

QString Dataset::dumpDatasetDefinition() const
{
    QString dump;
    for (unsigned int i = 0; i < columnsCount_; ++i)
    {
        dump += "Column " + QString::number(i) +
                " name=" + headerColumnNames_.at(i);
        dump +=
            " format=" + QString::number(static_cast<int>(columnTypes_.at(i)));

        QMapIterator<SpecialColumn, unsigned int> it(specialColumns_);
        if (it.findNext(i))
            dump += " special=" + QString::number(static_cast<int>(it.key()));
        dump.append(QLatin1String("\n"));
    }
    return dump;
}

void Dataset::rebuildDefinitonUsingActiveColumnsOnly()
{
    QVector<ColumnType> tempColumnsFormat;
    QStringList tempHeaderColumnNames;
    QMap<SpecialColumn, unsigned int> specialColumnsTemp;
    int activeColumnNumber{0};
    const bool specialColumnDateMarked{
        isSpecialColumnTagged(SpecialColumn::TRANSACTION_DATE)};
    const bool specialColumnPriceMarked{
        isSpecialColumnTagged(SpecialColumn::PRICE_PER_UNIT)};

    for (unsigned int i = 0;
         i < static_cast<unsigned int>(activeColumns_.count()); ++i)
    {
        if (activeColumns_.at(i))
        {
            tempColumnsFormat.push_back(columnTypes_[i]);
            tempHeaderColumnNames << headerColumnNames_[i];
            if (specialColumnDateMarked &&
                specialColumns_.value(SpecialColumn::TRANSACTION_DATE) == i)
                specialColumnsTemp[SpecialColumn::TRANSACTION_DATE] =
                    activeColumnNumber;
            if (specialColumnPriceMarked &&
                specialColumns_.value(SpecialColumn::PRICE_PER_UNIT) == i)
                specialColumnsTemp[SpecialColumn::PRICE_PER_UNIT] =
                    activeColumnNumber;
            activeColumnNumber++;
        }
    }

    columnTypes_ = tempColumnsFormat;
    headerColumnNames_ = tempHeaderColumnNames;
    specialColumns_ = specialColumnsTemp;
    columnsCount_ = activeColumns_.count(true);
    activeColumns_.clear();
}

void Dataset::updateSampleDataStrings(QVector<QVector<QVariant>>& data) const
{
    if (sharedStrings_.isEmpty())
        return;

    for (unsigned int i = 0; i < columnCount(); ++i)
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

bool Dataset::isSpecialColumnTagged(SpecialColumn column) const
{
    return specialColumns_.contains(column);
}
