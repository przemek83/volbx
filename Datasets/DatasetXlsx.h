#ifndef DATASETXLSX_H
#define DATASETXLSX_H

#include "DatasetSpreadsheet.h"

/**
 * @brief Dataset definition for .xlsx files.
 */
class DatasetXlsx : public DatasetSpreadsheet
{
    Q_OBJECT
public:
    DatasetXlsx(const QString& name, QString& zipFileName,
                QObject* parent = nullptr);

    ~DatasetXlsx() override = default;

protected:
    bool loadSpecificData() override;

private:
    bool loadSharedStrings();
};

#endif  // DATASETXLSX_H
