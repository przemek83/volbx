#include "DatasetDefinitionXlsx.h"

#include <cmath>
#include <future>
#include <memory>

#include <QApplication>
#include <QDebug>
#include <QDomDocument>
#include <QFuture>
#include <QVariant>

#include "Common/Constants.h"
#include "Shared/Logger.h"

DatasetDefinitionXlsx::DatasetDefinitionXlsx(const QString& name,
                                             QString& zipFileName)
    : DatasetDefinitionSpreadsheet(name, zipFileName)
{
    importer_ = std::make_unique<ImportXlsx>(zipFile_);
}

bool DatasetDefinitionXlsx::getSheetList()
{
    bool success{false};
    std::tie(success, sheets_) = importer_->getSheetNames();
    if (!success)
        LOG(LogTypes::IMPORT_EXPORT, importXlsx_.getLastError());
    return success;
}

bool DatasetDefinitionXlsx::loadSharedStrings()
{
    auto [success, sharedStringsList] =
        dynamic_cast<ImportXlsx*>(importer_.get())->getSharedStrings();
    if (!success)
    {
        LOG(LogTypes::IMPORT_EXPORT, importXlsx_.getLastError());
        return false;
    }
    for (const auto& sharedString : sharedStringsList)
    {
        stringsMap_[sharedString] = nextSharedStringIndex_;
        nextSharedStringIndex_++;
    }
    return success;
}

bool DatasetDefinitionXlsx::getColumnList(const QString& sheetName)
{
    bool success{false};
    std::tie(success, headerColumnNames_) =
        importer_->getColumnNames(sheetName);
    if (!success)
        LOG(LogTypes::IMPORT_EXPORT, importXlsx_.getLastError());
    return success;
}

bool DatasetDefinitionXlsx::getColumnTypes(const QString& sheetName)
{
    bool success{false};
    std::tie(success, columnTypes_) = importer_->getColumnTypes(sheetName);
    if (!success)
    {
        LOG(LogTypes::IMPORT_EXPORT, importXlsx_.getLastError());
        return false;
    }
    rowsCount_ = static_cast<int>(importer_->getRowCount(sheetName).second);
    return true;
}

bool DatasetDefinitionXlsx::getDataFromZip(
    const QString& sheetName, QVector<QVector<QVariant> >* dataContainer,
    bool fillSamplesOnly)
{
    bool success{false};
    if (fillSamplesOnly)
    {
        std::tie(success, *dataContainer) = importer_->getLimitedData(
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
            importer_->getData(sheetName, excludedColumns);
    }

    if (!success)
    {
        LOG(LogTypes::IMPORT_EXPORT, importXlsx_.getLastError());
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

const QString& DatasetDefinitionXlsx::getSheetName() { return sheets_.front(); }

bool DatasetDefinitionXlsx::loadSpecificData()
{
    if (!loadSharedStrings())
    {
        error_ = QObject::tr("File ") + zipFile_.fileName() +
                 QObject::tr(" is damaged.");
        return false;
    }

    return true;
}
