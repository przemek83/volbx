#pragma once

#include <QObject>

#include "Datasets/DatasetSpreadsheet.h"

/**
 * @brief Unit test for spreadsheets.
 */
class SpreadsheetsTest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void testCompareSpreadsheetFiles_data();
    void testCompareSpreadsheetFiles();

    void testCompareSpreadsheetFilesDamaged_data();
    void testCompareSpreadsheetFilesDamaged();

    void testCompareDefinitionDumps_data();
    void testCompareDefinitionDumps();

    void testCompareTsvDumps_data();
    void testCompareTsvDumps();

private:
    QVector<QString> fileNames_{"excel",
                                "HistVsNormal",
                                "import1",
                                "import2",
                                "import3",
                                "smallDataSet",
                                "test2",
                                "testAccounts",
                                "testDataWithPlot",
                                "testDataWithPlot",
                                "test"};

    std::unique_ptr<DatasetSpreadsheet> createDataset(const QString& fileName);

    void addTestDataForDumpsComparison(const QString& testNamePrefix);

    void compareDumps(const QString& fileSuffix);

    void compareDatasetDefinitionWithDump(
        const std::unique_ptr<DatasetSpreadsheet>& dataset,
        const QString& fileName);

    void activateAllDatasetColumns(
        const std::unique_ptr<DatasetSpreadsheet>& dataset);

    void generateDataDumpsForFile(QString name);

    void generateAllDumpData();

    QString getSpreadsheetsDir();

    void addTestCasesForFileNames(const QVector<QString>& fileNames);
};
