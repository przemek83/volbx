#ifndef DATASETDEFINITIONXLSX_H
#define DATASETDEFINITIONXLSX_H

#include <QXmlStreamReader>
#include <quazip5/quazip.h>
#include <quazip5/quazipfile.h>

#include "DatasetDefinitionSpreadsheet.h"

/**
 * @brief Dataset definition for .xlsx files.
 */
class DatasetDefinitionXlsx : public DatasetDefinitionSpreadsheet
{
public:
    DatasetDefinitionXlsx(const QString& name, QString& zipFileName);

    ~DatasetDefinitionXlsx() override = default;

protected:
    const QString& getSheetName() override;

    bool getDataFromZip(QuaZip& zip, const QString& sheetName,
                        QVector<QVector<QVariant>>* dataContainer,
                        bool fillSamplesOnly) override;

private:
    Q_DISABLE_COPY(DatasetDefinitionXlsx)

    bool getSheetList(QuaZip& zip) override;

    bool getColumnList(QuaZip& zip, const QString& sheetName) override;

    bool loadStyles(QuaZip& zip);

    bool loadSharedStrings(QuaZip& zip);

    bool getColumnTypes(QuaZip& zip, const QString& sheetName) override;

    bool openZipAndMoveToSecondRow(QuaZip& zip,
                                   const QString& sheetName,
                                   QuaZipFile& zipFile,
                                   QXmlStreamReader& xmlStreamReader);

    bool loadSpecificData(QuaZip& zip) override;

    ///Map sheet <-> fileName used to access sheets in zip.
    QMap<QString, QString> sheetToFileMapInZip_;

    ///List of date styles defined in excel.
    QList<int> dateStyles_;

    ///List of styles defined in excel.
    QList<int> allStyles_;

    ///Column list for excel (A, B, C, ...).
    QStringList excelColNames_;
};

#endif // DATASETDEFINITIONXLSX_H
