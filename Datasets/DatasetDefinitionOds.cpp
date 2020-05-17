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

bool DatasetDefinitionOds::loadSpecificData()
{
    // Nothing specific for .ods.
    return true;
}
