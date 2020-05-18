#ifndef DATASETXLSX_H
#define DATASETXLSX_H

#include <ImportXlsx.h>
#include <Qt5Quazip/quazip.h>
#include <Qt5Quazip/quazipfile.h>
#include <QXmlStreamReader>

#include "DatasetSpreadsheet.h"

class ImportXlsx;

/**
 * @brief Dataset definition for .xlsx files.
 */
class DatasetXlsx : public DatasetSpreadsheet
{
    Q_OBJECT
public:
    DatasetXlsx(const QString& name, QString& zipFileName);

    ~DatasetXlsx() override = default;

protected:
    bool loadSpecificData() override;

private:
    bool loadSharedStrings();
};

#endif  // DATASETXLSX_H
