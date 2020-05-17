#ifndef DATASETDEFINITIONSPREADSHEET_H
#define DATASETDEFINITIONSPREADSHEET_H

#include <QHash>

#include <Qt5Quazip/quazip.h>

#include "DatasetDefinition.h"

class ImportSpreadsheet;

/**
 * @brief Dataset definition for spreadsheet files.
 */
class DatasetDefinitionSpreadsheet : public DatasetDefinition
{
    Q_OBJECT
public:
    DatasetDefinitionSpreadsheet(const QString& name,
                                 const QString& zipFileName);

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

    const QString& getSheetName();

    virtual bool getDataFromZip(const QString& sheetName,
                                QVector<QVector<QVariant> >* dataContainer,
                                bool fillSamplesOnly) = 0;

    virtual bool loadSpecificData() = 0;

    /// Temporary string <-> index map used to build shared string table.
    QHash<QString, int> stringsMap_;

    /// Next index to be used in strings hash map.
    int nextSharedStringIndex_{0};

    QFile zipFile_;
    std::unique_ptr<ImportSpreadsheet> importer_{nullptr};

private:
    bool getSheetList();
    bool getColumnList(const QString& sheetName);
    bool getColumnTypes(const QString& sheetName);

    QStringList sheetNames_;
};

#endif  // DATASETDEFINITIONSPREADSHEET_H
