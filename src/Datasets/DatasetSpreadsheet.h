#pragma once

#include <eible/ImportSpreadsheet.h>
#include <QFile>

#include "Dataset.h"

class DatasetSpreadsheet : public Dataset
{
    Q_OBJECT
public:
    DatasetSpreadsheet(const QString& name, const QString& zipFileName);

protected:
    bool analyze() override;

    virtual bool loadSpecificData() = 0;

    std::tuple<bool, QVector<QVector<QVariant>>> getSample() override;

    std::tuple<bool, QVector<QVector<QVariant>>> getAllData() override;

    void closeZip() override;

    QFile zipFile_;
    std::unique_ptr<ImportSpreadsheet> importer_{nullptr};

private:
    bool getSheetList();
    bool getHeadersList(const QString& sheetName);
    bool getColumnTypes(const QString& sheetName);
    std::tuple<bool, QVector<QVector<QVariant>>> getDataFromZip(
        const QString& sheetName, bool fillSamplesOnly);

    const QString& getSheetName() const;

    QStringList sheetNames_;
};
