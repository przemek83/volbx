#include "DatasetSpreadsheet.h"

#include <quazip5/quazipfile.h>
#include <QDebug>

#include "Common/Constants.h"
#include "Shared/Logger.h"

#include "DatasetDefinitionSpreadsheet.h"

DatasetSpreadsheet::DatasetSpreadsheet(
    DatasetDefinitionSpreadsheet* datasetDefinition)
    : Dataset(datasetDefinition)
{
}

void DatasetSpreadsheet::init()
{
    // Load data if definition is valid.
    if (datasetDefinition_->isValid())
    {
        auto definition =
            dynamic_cast<DatasetDefinitionSpreadsheet*>(datasetDefinition_);

        data_.resize(rowCount());
        if (nullptr != definition && definition->getData(&data_))
        {
            valid_ = true;
            sharedStrings_ = definition->getSharedStringTable();
        }
    }
}
