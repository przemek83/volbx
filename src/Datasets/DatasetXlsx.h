#pragma once

#include "DatasetSpreadsheet.h"

class DatasetXlsx : public DatasetSpreadsheet
{
    Q_OBJECT
public:
    DatasetXlsx(const QString& name, const QString& zipFileName);

protected:
    bool loadSpecificData() override;

private:
    bool loadSharedStrings();
};
