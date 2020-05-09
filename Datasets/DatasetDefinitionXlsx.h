#ifndef DATASETDEFINITIONXLSX_H
#define DATASETDEFINITIONXLSX_H

#include <ImportXlsx.h>
#include <quazip5/quazip.h>
#include <quazip5/quazipfile.h>
#include <QXmlStreamReader>

#include "DatasetDefinitionSpreadsheet.h"

class ImportXlsx;

/**
 * @brief Dataset definition for .xlsx files.
 */
class DatasetDefinitionXlsx : public DatasetDefinitionSpreadsheet
{
public:
    DatasetDefinitionXlsx(const QString& name, QString& zipFileName);

    ~DatasetDefinitionXlsx() override = default;

    DatasetDefinitionXlsx& operator=(const DatasetDefinitionXlsx& other) =
        delete;
    DatasetDefinitionXlsx(const DatasetDefinitionXlsx& other) = delete;

    DatasetDefinitionXlsx& operator=(DatasetDefinitionXlsx&& other) = delete;
    DatasetDefinitionXlsx(DatasetDefinitionXlsx&& other) = delete;

protected:
    const QString& getSheetName() override;

    bool getDataFromZip(QuaZip& zip, const QString& sheetPath,
                        QVector<QVector<QVariant>>* dataContainer,
                        bool fillSamplesOnly) override;

    bool getSheetList(QuaZip& zip) override;

    bool getColumnList(QuaZip& zip, const QString& sheetName) override;

    bool getColumnTypes(QuaZip& zip, const QString& sheetPath) override;

    bool loadSpecificData(QuaZip& zip) override;

private:
    bool loadSharedStrings(ImportXlsx& importXlsx);

    QFile xlsxFile_;
    ImportXlsx importXlsx_;
    QStringList sheets_;
};

#endif  // DATASETDEFINITIONXLSX_H
