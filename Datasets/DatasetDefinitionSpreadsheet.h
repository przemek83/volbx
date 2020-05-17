#ifndef DATASETDEFINITIONSPREADSHEET_H
#define DATASETDEFINITIONSPREADSHEET_H

#include <QHash>

#include <Qt5Quazip/quazip.h>

#include "DatasetDefinition.h"

/**
 * @brief Dataset definition for spreadsheet files.
 */
class DatasetDefinitionSpreadsheet : public DatasetDefinition
{
    Q_OBJECT
public:
    explicit DatasetDefinitionSpreadsheet(const QString& name);

    ~DatasetDefinitionSpreadsheet() override = default;

    DatasetDefinitionSpreadsheet& operator=(
        const DatasetDefinitionSpreadsheet& other) = delete;
    DatasetDefinitionSpreadsheet(const DatasetDefinitionSpreadsheet& other) =
        delete;

    DatasetDefinitionSpreadsheet& operator=(
        DatasetDefinitionSpreadsheet&& other) = delete;
    DatasetDefinitionSpreadsheet(DatasetDefinitionSpreadsheet&& other) = delete;

    bool isValid() const override;

    bool init();

    virtual bool getData(QVector<QVector<QVariant> >* dataContainer);

    std::unique_ptr<QVariant[]> getSharedStringTable() override;

protected:
    void updateSampleDataStrings();

    virtual const QString& getSheetName() = 0;

    virtual bool getDataFromZip(const QString& sheetName,
                                QVector<QVector<QVariant> >* dataContainer,
                                bool fillSamplesOnly) = 0;

    virtual bool getSheetList() = 0;

    virtual bool getColumnList(const QString& sheetName) = 0;

    virtual bool loadSpecificData() = 0;

    virtual bool getColumnTypes(const QString& sheetName) = 0;

    /// Temporary string <-> index map used to build shared string table.
    QHash<QString, int> stringsMap_;

    /// Next index to be used in strings hash map.
    int nextSharedStringIndex_{0};
};

#endif  // DATASETDEFINITIONSPREADSHEET_H
