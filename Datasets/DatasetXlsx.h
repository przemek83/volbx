#pragma once

#include "DatasetSpreadsheet.h"

/**
 * @class DatasetXlsx
 * @brief Dataset definition for .xlsx files.
 */
class DatasetXlsx : public DatasetSpreadsheet
{
    Q_OBJECT
public:
    DatasetXlsx(const QString& name, const QString& zipFileName,
                QObject* parent = nullptr);

protected:
    bool loadSpecificData() override;

private:
    bool loadSharedStrings();
};
