#ifndef DATASETDEFINITIONODS_H
#define DATASETDEFINITIONODS_H

#include <ImportOds.h>
#include <Qt5Quazip/quazip.h>
#include <Qt5Quazip/quazipfile.h>
#include <QXmlStreamReader>

#include "DatasetDefinitionSpreadsheet.h"

/**
 * @brief Dataset definition for .ods files.
 */
class DatasetDefinitionOds : public DatasetDefinitionSpreadsheet
{
    Q_OBJECT
public:
    DatasetDefinitionOds(const QString& name, const QString& zipFileName);

    ~DatasetDefinitionOds() override = default;

    DatasetDefinitionOds& operator=(const DatasetDefinitionOds& other) = delete;
    DatasetDefinitionOds(const DatasetDefinitionOds& other) = delete;

    DatasetDefinitionOds& operator=(DatasetDefinitionOds&& other) = delete;
    DatasetDefinitionOds(DatasetDefinitionOds&& other) = delete;

protected:
    const QString& getSheetName() override;

    bool getDataFromZip(const QString& sheetName,
                        QVector<QVector<QVariant>>* dataContainer,
                        bool fillSamplesOnly) override;

    bool getSheetList() override;

    bool getColumnList(const QString& sheetName) override;

    bool loadSpecificData() override;

    bool getColumnTypes(const QString& sheetName) override;

private:
    QStringList sheetNames_;
};

#endif  // DATASETDEFINITIONODS_H
