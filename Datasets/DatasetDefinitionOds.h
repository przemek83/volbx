#ifndef DATASETDEFINITIONODS_H
#define DATASETDEFINITIONODS_H

#include <quazip5/quazip.h>
#include <QXmlStreamReader>
#include <quazip5/quazipfile.h>

#include "DatasetDefinitionSpreadsheet.h"

/**
 * @brief Dataset definition for .ods files.
 */
class DatasetDefinitionOds : public DatasetDefinitionSpreadsheet
{
public:
    DatasetDefinitionOds(const QString& name, const QString& zipFileName);

    ~DatasetDefinitionOds() override = default;

    DatasetDefinitionOds& operator=(const DatasetDefinitionOds& other) = delete;
    DatasetDefinitionOds(const DatasetDefinitionOds& other) = delete;

    DatasetDefinitionOds& operator=(DatasetDefinitionOds&& other) = delete;
    DatasetDefinitionOds(DatasetDefinitionOds&& other) = delete;

protected:
    const QString& getSheetName() override;

    bool getDataFromZip(QuaZip& zip, const QString& sheetName,
                        QVector<QVector<QVariant>>* dataContainer,
                        bool fillSamplesOnly) override;

private:
    bool getSheetList(QuaZip& zip) override;

    bool getColumnList(QuaZip& zip, const QString& sheetName) override;

    bool loadSpecificData(QuaZip& zip) override;

    bool getColumnTypes(QuaZip& zip, const QString& sheetName) override;

    bool openZipAndMoveToSecondRow(QuaZip& zip,
                                   const QString& sheetName,
                                   QuaZipFile& zipFile,
                                   QXmlStreamReader& xmlStreamReader);

    QStringList sheetNames_;
};

#endif // DATASETDEFINITIONODS_H
