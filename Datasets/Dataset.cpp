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
        double value{data_[i][column].toDouble()};
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

std::tuple<bool, unsigned int> Dataset::getSpecialColumnIfExists(
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
    if (auto [ok, column] =
            getSpecialColumnIfExists(SPECIAL_COLUMN_PRICE_PER_UNIT);
        ok)
        tabName.append(" (" + getHeaderName(column) + ")");
    return tabName;
}

QByteArray Dataset::definitionToXml(unsigned int rowCount) const
{
    QDomDocument xmlDocument(DATASET_NAME);
    QDomElement root{xmlDocument.createElement(DATASET_NAME)};
    xmlDocument.appendChild(root);
    QDomElement columns{xmlDocument.createElement(DATASET_COLUMNS)};
    root.appendChild(columns);
    for (unsigned int i = 0; i < columnsCount_; ++i)
    {
        QDomElement node{xmlDocument.createElement(DATASET_COLUMN)};
        node.setAttribute(DATASET_COLUMN_NAME, headerColumnNames_.at(i));
        node.setAttribute(DATASET_COLUMN_FORMAT,
                          static_cast<int>(columnTypes_.at(i)));
        QMapIterator<SpecialColumn, unsigned int> it(specialColumns_);
        if (it.findNext(i))
            node.setAttribute(DATASET_COLUMN_SPECIAL_TAG,
                              QString::number(static_cast<int>(it.key())));
        columns.appendChild(node);
    }

    QDomElement rowCountElement{xmlDocument.createElement(DATASET_ROW_COUNT)};
    rowCountElement.setAttribute(DATASET_ROW_COUNT, QString::number(rowCount));
    root.appendChild(rowCountElement);
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
        if (!activeColumns_.isEmpty())
        {
            const QString notActive(QStringLiteral(" not active"));
            const QString active(QStringLiteral(" active"));
            dump += " active=" + (activeColumns_[i] ? active : notActive);
        }

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
        isSpecialColumnTagged(SPECIAL_COLUMN_TRANSACTION_DATE)};
    const bool specialColumnPriceMarked{
        isSpecialColumnTagged(SPECIAL_COLUMN_PRICE_PER_UNIT)};

    for (unsigned int i = 0;
         i < static_cast<unsigned int>(activeColumns_.count()); ++i)
    {
        if (activeColumns_.at(i))
        {
            tempColumnsFormat.push_back(columnTypes_[i]);
            tempHeaderColumnNames << headerColumnNames_[i];
            if (specialColumnDateMarked &&
                specialColumns_.value(SPECIAL_COLUMN_TRANSACTION_DATE) == i)
                specialColumnsTemp[SPECIAL_COLUMN_TRANSACTION_DATE] =
                    activeColumnNumber;
            if (specialColumnPriceMarked &&
                specialColumns_.value(SPECIAL_COLUMN_PRICE_PER_UNIT) == i)
                specialColumnsTemp[SPECIAL_COLUMN_PRICE_PER_UNIT] =
                    activeColumnNumber;
            activeColumnNumber++;
        }
    }

    columnTypes_ = tempColumnsFormat;
    headerColumnNames_ = tempHeaderColumnNames;
    specialColumns_ = specialColumnsTemp;
    columnsCount_ = getActiveColumnCount();
    activeColumns_.clear();
}

unsigned int Dataset::getActiveColumnCount() const
{
    return activeColumns_.count(true);
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
