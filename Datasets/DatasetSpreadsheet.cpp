#include <QDebug>
#include <quazip5/quazipfile.h>

#include "Common/Constants.h"
#include "Shared/Logger.h"

#include "DatasetSpreadsheet.h"
#include "DatasetDefinitionSpreadsheet.h"

DatasetSpreadsheet::DatasetSpreadsheet(DatasetDefinitionSpreadsheet* datasetDefinition)
    : Dataset(datasetDefinition)
{

}

DatasetSpreadsheet::~DatasetSpreadsheet()
{

}

void DatasetSpreadsheet::init()
{
    //Load data if definition is valid.
    if( datasetDefinition_->isValid() )
    {
        DatasetDefinitionSpreadsheet* definition =
                dynamic_cast<DatasetDefinitionSpreadsheet*>(datasetDefinition_);

        data_.resize(rowCount());
        if( nullptr != definition && true == definition->getData(&data_) )
        {
            valid_ = true;
            sharedStrings_ = definition->getSharedStringTable();
        }
    }
}
