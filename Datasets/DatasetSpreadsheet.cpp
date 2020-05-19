#include "DatasetSpreadsheet.h"

#include <ImportSpreadsheet.h>

#include "Shared/Logger.h"

DatasetSpreadsheet::DatasetSpreadsheet(const QString& name,
                                       const QString& zipFileName,
                                       QObject* parent)
    : Dataset(name, parent), zipFile_(zipFileName)
{
}

DatasetSpreadsheet::~DatasetSpreadsheet() {}

bool DatasetSpreadsheet::analyze()
{
    importer_->setNameForEmptyColumn(QObject::tr("no name"));
    QObject::connect(importer_.get(),
                     &ImportSpreadsheet::progressPercentChanged, this,
                     &Dataset::loadingPercentChanged);

    if (!getSheetList() || !loadSpecificData() ||
        !getColumnTypes(getSheetName()) || !getHeadersList(getSheetName()))
        return false;

    columnsCount_ = importer_->getColumnCount(getSheetName()).second;
    rowsCount_ = importer_->getRowCount(getSheetName()).second;

    if (!prepareSampleData())
        return false;

    valid_ = true;

    return true;
}

void DatasetSpreadsheet::updateSampleDataStrings()
{
    if (stringsMap_.isEmpty())
        return;

    for (int i = 0; i < columnCount(); ++i)
        if (ColumnType::STRING == columnTypes_.at(i))
            for (auto& sampleDataRow : sampleData_)
                sampleDataRow[i] = stringsMap_.key(sampleDataRow[i].toInt());
}

const QString& DatasetSpreadsheet::getSheetName()
{
    return sheetNames_.constFirst();
}

bool DatasetSpreadsheet::prepareSampleData()
{
    sampleData_.resize(SAMPLE_SIZE < rowCount() ? SAMPLE_SIZE : rowCount());
    if (!getDataFromZip(getSheetName(), sampleData_, true))
        return false;

    updateSampleDataStrings();
    return true;
}

bool DatasetSpreadsheet::getSheetList()
{
    bool success{false};
    std::tie(success, sheetNames_) = importer_->getSheetNames();
    if (!success)
        LOG(LogTypes::IMPORT_EXPORT, importXlsx_.getLastError());
    return success;
}

bool DatasetSpreadsheet::getHeadersList(const QString& sheetName)
{
    bool success{false};
    std::tie(success, headerColumnNames_) =
        importer_->getColumnNames(sheetName);
    if (!success)
        LOG(LogTypes::IMPORT_EXPORT, importXlsx_.getLastError());
    return success;
}

bool DatasetSpreadsheet::getColumnTypes(const QString& sheetName)
{
    bool success{false};
    std::tie(success, columnTypes_) = importer_->getColumnTypes(sheetName);
    if (!success)
    {
        LOG(LogTypes::IMPORT_EXPORT, importOds_.getLastError());
        return false;
    }
    return true;
}

bool DatasetSpreadsheet::getDataFromZip(
    const QString& sheetName, QVector<QVector<QVariant> >& dataContainer,
    bool fillSamplesOnly)
{
    bool success{false};
    if (fillSamplesOnly)
    {
        std::tie(success, dataContainer) = importer_->getLimitedData(
            sheetName, {}, SAMPLE_SIZE < rowCount() ? SAMPLE_SIZE : rowCount());
    }
    else
    {
        QVector<unsigned int> excludedColumns;
        for (int i = 0; i < columnCount(); ++i)
            if (!activeColumns_.at(i))
                excludedColumns.append(i);
        std::tie(success, dataContainer) =
            importer_->getData(sheetName, excludedColumns);
    }

    if (!success)
    {
        LOG(LogTypes::IMPORT_EXPORT, importXlsx_.getLastError());
        return false;
    }

    if (!fillSamplesOnly)
    {
        Q_ASSERT(rowCount() == dataContainer.size());
        LOG(LogTypes::IMPORT_EXPORT,
            "Loaded file having " + QString::number(rowsCount_) + " rows.");
    }

    return true;
}

std::unique_ptr<QVariant[]> DatasetSpreadsheet::getSharedStringTable()
{
    auto stringsTable = std::make_unique<QVariant[]>(
        static_cast<size_t>(nextSharedStringIndex_));
    auto it = stringsMap_.constBegin();
    while (it != stringsMap_.constEnd())
    {
        stringsTable[static_cast<size_t>(it.value())] = QVariant(it.key());
        ++it;
    }
    stringsMap_.clear();
    return stringsTable;
}

bool DatasetSpreadsheet::loadData()
{
    if (!isValid())
        return false;

    data_.resize(rowCount());

    const bool result{getDataFromZip(getSheetName(), data_, false)};
    if (result)
    {
        valid_ = true;
        sharedStrings_ = getSharedStringTable();
    }
    rebuildDefinitonUsingActiveColumnsOnly();
    return result;
}
