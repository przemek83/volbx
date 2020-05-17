#include "DatasetDefinitionSpreadsheet.h"

#include <QDebug>

#include "Shared/Logger.h"

DatasetDefinitionSpreadsheet::DatasetDefinitionSpreadsheet(const QString& name)
    : DatasetDefinition(name)
{
}

bool DatasetDefinitionSpreadsheet::init()
{
    if (!getSheetList() || !loadSpecificData() ||
        !getColumnList(getSheetName()))
        return false;

    columnsCount_ = headerColumnNames_.size();

    if (!getColumnTypes(getSheetName()))
        return false;

    // Sample data.
    sampleData_.resize(SAMPLE_SIZE < rowsCount_ ? SAMPLE_SIZE : rowsCount_);
    if (!getDataFromZip(getSheetName(), &sampleData_, true))
        return false;

    // Set proper strings for sample data.
    updateSampleDataStrings();
    valid_ = true;

    return true;
}

void DatasetDefinitionSpreadsheet::updateSampleDataStrings()
{
    if (stringsMap_.isEmpty())
        return;

    for (int i = 0; i < columnsCount_; ++i)
    {
        if (ColumnType::STRING == columnTypes_.at(i))
        {
            for (auto& sampleDataRow : sampleData_)
            {
                sampleDataRow[i] = stringsMap_.key(sampleDataRow[i].toInt());
            }
        }
    }
}

bool DatasetDefinitionSpreadsheet::isValid() const { return valid_; }

std::unique_ptr<QVariant[]> DatasetDefinitionSpreadsheet::getSharedStringTable()
{
    auto stringsTable = std::make_unique<QVariant[]>(
        static_cast<size_t>(nextSharedStringIndex_));
    QHash<QString, int>::const_iterator i = stringsMap_.constBegin();
    while (i != stringsMap_.constEnd())
    {
        stringsTable[static_cast<size_t>(i.value())] = QVariant(i.key());
        ++i;
    }

    stringsMap_.clear();

    return stringsTable;
}

bool DatasetDefinitionSpreadsheet::getData(
    QVector<QVector<QVariant> >* dataContainer)
{
    const bool result{getDataFromZip(getSheetName(), dataContainer, false)};
    rebuildDefinitonUsingActiveColumnsOnly();
    return result;
}
