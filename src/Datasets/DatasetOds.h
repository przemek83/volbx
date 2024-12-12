#pragma once

#include "DatasetSpreadsheet.h"

/**
 * @class DatasetOds
 * @brief Dataset definition for .ods files.
 */
class DatasetOds : public DatasetSpreadsheet
{
    Q_OBJECT
public:
    DatasetOds(const QString& name, const QString& zipFileName,
               QObject* parent = nullptr);

protected:
    bool loadSpecificData() override;
};
