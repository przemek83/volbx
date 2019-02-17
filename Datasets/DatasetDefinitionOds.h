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
    DatasetDefinitionOds(const QString& name, QString& zipFileName);

    virtual ~DatasetDefinitionOds() = default;

protected:
    virtual const QString& getSheetName();

    virtual bool getDataFromZip( QuaZip& zip,
                                 const QString& sheetName,
                                 QVector<QVector<QVariant> >* dataContainer,
                                 bool fillSamplesOnly );
private:
    Q_DISABLE_COPY(DatasetDefinitionOds)

    virtual bool getSheetList(QuaZip& zip);

    virtual bool getColumnList(QuaZip& zip,
                               const QString& sheetName);

    virtual bool loadSpecificData(QuaZip& zip);

    virtual bool getColumnTypes( QuaZip& zip,
                                 const QString& sheetName );

    bool openZipAndMoveToSecondRow(QuaZip& zip,
                                   const QString& sheetName,
                                   QuaZipFile& zipFile,
                                   QXmlStreamReader& xmlStreamReader);

    QStringList sheetNames_;
};

#endif // DATASETDEFINITIONODS_H
