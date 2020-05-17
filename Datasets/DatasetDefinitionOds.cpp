#include "DatasetDefinitionOds.h"

#include <future>
#include <memory>

#include <Qt5Quazip/quazipfile.h>
#include <QApplication>
#include <QDebug>
#include <QDomDocument>
#include <QDomElement>
#include <QXmlStreamReader>

#include "Common/Constants.h"
#include "Shared/Logger.h"

DatasetDefinitionOds::DatasetDefinitionOds(const QString& name,
                                           const QString& zipFileName)
    : DatasetDefinitionSpreadsheet(name),
      odsFile_(zipFileName),
      importOds_(odsFile_)
{
    importOds_.setNameForEmptyColumn(QObject::tr("no name"));
    QObject::connect(&importOds_, &ImportSpreadsheet::progressPercentChanged,
                     this, &DatasetDefinition::loadingPercentChanged);
}

bool DatasetDefinitionOds::getSheetList()
{
    auto [success, sheetNames] = importOds_.getSheetNames();
    if (!success)
        LOG(LogTypes::IMPORT_EXPORT, importOds_.getLastError());
    sheetNames_ = std::move(sheetNames);
    return success;
}

bool DatasetDefinitionOds::getColumnList(const QString& sheetName)
{
    auto [success, columnNames] = importOds_.getColumnNames(sheetName);
    if (!success)
        LOG(LogTypes::IMPORT_EXPORT, importOds_.getLastError());
    headerColumnNames_ = std::move(columnNames);
    return success;
}

bool DatasetDefinitionOds::getColumnTypes(const QString& sheetName)
{
    bool success{false};
    std::tie(success, columnTypes_) = importOds_.getColumnTypes(sheetName);
    if (!success)
    {
        LOG(LogTypes::IMPORT_EXPORT, importOds_.getLastError());
        return false;
    }
    rowsCount_ = static_cast<int>(importOds_.getRowCount(sheetName).second);
    return true;
}

bool DatasetDefinitionOds::getDataFromZip(
    const QString& sheetName, QVector<QVector<QVariant> >* dataContainer,
    bool fillSamplesOnly)
{
    bool success{false};
    if (fillSamplesOnly)
    {
        std::tie(success, *dataContainer) = importOds_.getLimitedData(
            sheetName, {}, SAMPLE_SIZE < rowsCount_ ? SAMPLE_SIZE : rowsCount_);
    }
    else
    {
        QVector<unsigned int> excludedColumns;
        for (int i = 0; i < columnsCount_; ++i)
        {
            if (!activeColumns_.at(i))
                excludedColumns.append(i);
        }
        std::tie(success, *dataContainer) =
            importOds_.getData(sheetName, excludedColumns);
    }

    if (!success)
    {
        LOG(LogTypes::IMPORT_EXPORT, importOds_.getLastError());
        return false;
    }

    if (!fillSamplesOnly)
    {
        Q_ASSERT(rowsCount_ == dataContainer->size());
        LOG(LogTypes::IMPORT_EXPORT,
            "Loaded file having " + QString::number(rowsCount_) + " rows.");
    }

    return true;
}

const QString& DatasetDefinitionOds::getSheetName()
{
    return sheetNames_.constFirst();
}

bool DatasetDefinitionOds::loadSpecificData()
{
    // Nothing specific for .ods.
    return true;
}
