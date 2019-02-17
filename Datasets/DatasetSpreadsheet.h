#ifndef DATASETSPREADSHEET_H
#define DATASETSPREADSHEET_H

#include <quazip5/quazip.h>

#include "Dataset.h"

/**
 * @brief Dataset class for spreadsheets.
 */
class DatasetDefinitionSpreadsheet;

class DatasetSpreadsheet : public Dataset
{
public:
    explicit DatasetSpreadsheet(DatasetDefinitionSpreadsheet* datasetDefinition);

    virtual ~DatasetSpreadsheet() = default;

    virtual void init();

private:
    Q_DISABLE_COPY(DatasetSpreadsheet)
};

#endif // DATASETSPREADSHEET_H
