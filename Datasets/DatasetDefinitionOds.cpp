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
    : DatasetDefinitionSpreadsheet(name, zipFileName)
{
    importer_ = std::make_unique<ImportOds>(zipFile_);
}

bool DatasetDefinitionOds::getDataFromZip(
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

bool DatasetDefinitionOds::loadSpecificData()
{
    // Nothing specific for .ods.
    return true;
}
