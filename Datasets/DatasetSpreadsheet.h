#ifndef DATASETSPREADSHEET_H
#define DATASETSPREADSHEET_H

#include <Qt5Quazip/quazip.h>

#include "Dataset.h"

class ImportSpreadsheet;

/**
 * @brief Dataset class for spreadsheets.
 */
class DatasetSpreadsheet : public Dataset
{
public:
    explicit DatasetSpreadsheet(const QString& name, const QString& zipFileName,
                                QObject* parent = nullptr);

    ~DatasetSpreadsheet() override = default;

    bool loadData() override;

    bool analyze() override;

protected:
    std::unique_ptr<QVariant[]> getSharedStringTable() override;

    virtual bool loadSpecificData() = 0;

    /// Temporary string <-> index map used to build shared string table.
    QHash<QString, int> stringsMap_;

    /// Next index to be used in strings hash map.
    int nextSharedStringIndex_{0};

    QFile zipFile_;
    std::unique_ptr<ImportSpreadsheet> importer_{nullptr};

private:
    void updateSampleDataStrings();

    bool getSheetList();
    bool getHeadersList(const QString& sheetName);
    bool getColumnTypes(const QString& sheetName);
    bool getDataFromZip(const QString& sheetName,
                        QVector<QVector<QVariant> >& dataContainer,
                        bool fillSamplesOnly);

    const QString& getSheetName();

    QStringList sheetNames_;
};

#endif  // DATASETSPREADSHEET_H
