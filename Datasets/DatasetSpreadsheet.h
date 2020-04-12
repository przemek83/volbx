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
    explicit DatasetSpreadsheet(
        DatasetDefinitionSpreadsheet* datasetDefinition);

    ~DatasetSpreadsheet() override = default;

    DatasetSpreadsheet& operator=(const DatasetSpreadsheet& other) = delete;
    DatasetSpreadsheet(const DatasetSpreadsheet& other) = delete;

    DatasetSpreadsheet& operator=(DatasetSpreadsheet&& other) = delete;
    DatasetSpreadsheet(DatasetSpreadsheet&& other) = delete;

    void init() override;
};

#endif  // DATASETSPREADSHEET_H
