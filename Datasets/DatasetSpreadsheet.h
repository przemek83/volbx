#ifndef DATASETSPREADSHEET_H
#define DATASETSPREADSHEET_H

#include <QFile>

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

    ~DatasetSpreadsheet() override;

protected:
    bool analyze() override;

    std::unique_ptr<QVariant[]> getSharedStringTable() override;

    virtual bool loadSpecificData() = 0;

    std::tuple<bool, QVector<QVector<QVariant>>> getSample() override;

    std::tuple<bool, QVector<QVector<QVariant>>> getAllData() override;

    /// Temporary string <-> index map used to build shared string table.
    QHash<QString, int> stringsMap_;

    /// Next index to be used in strings hash map.
    int nextSharedStringIndex_{0};

    QFile zipFile_;
    std::unique_ptr<ImportSpreadsheet> importer_{nullptr};

private:
    void updateSampleDataStrings(QVector<QVector<QVariant>>& data);

    bool getSheetList();
    bool getHeadersList(const QString& sheetName);
    bool getColumnTypes(const QString& sheetName);
    std::tuple<bool, QVector<QVector<QVariant>>> getDataFromZip(
        const QString& sheetName, bool fillSamplesOnly);

    const QString& getSheetName();

    QStringList sheetNames_;
};

#endif  // DATASETSPREADSHEET_H
