#ifndef DATASETDEFINITIONXLSX_H
#define DATASETDEFINITIONXLSX_H

#include <ImportXlsx.h>
#include <Qt5Quazip/quazip.h>
#include <Qt5Quazip/quazipfile.h>
#include <QXmlStreamReader>

#include "DatasetDefinitionSpreadsheet.h"

class ImportXlsx;

/**
 * @brief Dataset definition for .xlsx files.
 */
class DatasetDefinitionXlsx : public DatasetDefinitionSpreadsheet
{
    Q_OBJECT
public:
    DatasetDefinitionXlsx(const QString& name, QString& zipFileName);

    ~DatasetDefinitionXlsx() override = default;

    DatasetDefinitionXlsx& operator=(const DatasetDefinitionXlsx& other) =
        delete;
    DatasetDefinitionXlsx(const DatasetDefinitionXlsx& other) = delete;

    DatasetDefinitionXlsx& operator=(DatasetDefinitionXlsx&& other) = delete;
    DatasetDefinitionXlsx(DatasetDefinitionXlsx&& other) = delete;

protected:
    bool getDataFromZip(const QString& sheetPath,
                        QVector<QVector<QVariant>>* dataContainer,
                        bool fillSamplesOnly) override;

    bool loadSpecificData() override;

private:
    bool loadSharedStrings();
};

#endif  // DATASETDEFINITIONXLSX_H
