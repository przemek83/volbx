#include "DatasetDefinitionSpreadsheet.h"

#include <QDebug>

#include "Shared/Logger.h"

DatasetDefinitionSpreadsheet::DatasetDefinitionSpreadsheet(
    const QString& name, const QString& zipFileName)
    : DatasetDefinition(name), nextSharedStringIndex_(0)
{
    zip_.setZipName(zipFileName);
}

bool DatasetDefinitionSpreadsheet::init()
{
    // Open archive.
    if (!zip_.open(QuaZip::mdUnzip))
    {
        error_ = QObject::tr("Can not open file ") + zip_.getZipName();
        LOG(LogTypes::IMPORT_EXPORT, "Can not open file " + zip_.getZipName());
        return false;
    }

    if (!getSheetList(zip_) || !loadSpecificData(zip_) ||
        !getColumnList(zip_, getSheetName()))
    {
        error_ = QObject::tr("File ") + zip_.getZipName() +
                 QObject::tr(" is damaged.");
        zip_.close();
        return false;
    }

    columnsCount_ = headerColumnNames_.size();

    if (!getColumnTypes(zip_, getSheetName()))
    {
        error_ = QObject::tr("File ") + zip_.getZipName() +
                 QObject::tr(" is damaged.");
        zip_.close();
        return false;
    }

    // Sample data.
    sampleData_.resize(SAMPLE_SIZE < rowsCount_ ? SAMPLE_SIZE : rowsCount_);
    if (!getDataFromZip(zip_, getSheetName(), &sampleData_, true))
    {
        error_ = QObject::tr("File ") + zip_.getZipName() +
                 QObject::tr(" is damaged.");
        zip_.close();
        return false;
    }

    // Set proper strings for sample data.
    updateSampleDataStrings();

    zip_.close();

    valid_ = true;

    return true;
}

void DatasetDefinitionSpreadsheet::updateSampleDataStrings()
{
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
    // Open archive.
    if (!zip_.open(QuaZip::mdUnzip))
    {
        LOG(LogTypes::IMPORT_EXPORT, "Can not open file " + zip_.getZipName());
        return false;
    }

    bool result = getDataFromZip(zip_, getSheetName(), dataContainer, false);

    zip_.close();

    rebuildDefinitonUsingActiveColumnsOnly();

    return result;
}
