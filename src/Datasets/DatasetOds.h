#pragma once

#include "DatasetSpreadsheet.h"

class DatasetOds : public DatasetSpreadsheet
{
    Q_OBJECT
public:
    DatasetOds(const QString& name, const QString& zipFileName);

protected:
    bool loadSpecificData() override;
};
