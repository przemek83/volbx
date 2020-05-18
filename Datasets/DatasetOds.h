#ifndef DATASETODS_H
#define DATASETODS_H

#include <ImportOds.h>
#include <Qt5Quazip/quazip.h>
#include <Qt5Quazip/quazipfile.h>
#include <QXmlStreamReader>

#include "DatasetSpreadsheet.h"

/**
 * @brief Dataset definition for .ods files.
 */
class DatasetOds : public DatasetSpreadsheet
{
    Q_OBJECT
public:
    DatasetOds(const QString& name, const QString& zipFileName);

    ~DatasetOds() override = default;

    DatasetOds& operator=(const DatasetOds& other) = delete;
    DatasetOds(const DatasetOds& other) = delete;

    DatasetOds& operator=(DatasetOds&& other) = delete;
    DatasetOds(DatasetOds&& other) = delete;

protected:
    bool loadSpecificData() override;
};

#endif  // DATASETODS_H
