#include "DatasetOds.h"

#include <eible/ImportOds.h>

DatasetOds::DatasetOds(const QString& name, const QString& zipFileName)
    : DatasetSpreadsheet(name, zipFileName)
{
    importer_ = std::make_unique<ImportOds>(zipFile_);
}

bool DatasetOds::loadSpecificData()
{
    // Nothing specific for .ods.
    return true;
}
