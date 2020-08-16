#pragma once

#include <QObject>

#include "Datasets/DatasetSpreadsheet.h"

class QTableView;

/**
 * @brief Standard tests for spreadsheets.
 */
class SpreadsheetsTest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void testFiles_data();
    void testFiles();

    void testDamagedFiles_data();
    void testDamagedFiles();

    void compareExpectedDefinitionsOfOdsAndXlsx_data();
    void compareExpectedDefinitionsOfOdsAndXlsx();

    void compareExpectedTsvDumpsOfOdsAndXlsx_data();
    void compareExpectedTsvDumpsOfOdsAndXlsx();

private:
    QVector<QString> testFileNames_{"excel",
                                    "HistVsNormal",
                                    "import1",
                                    "import2",
                                    "import3",
                                    "smallDataSet",
                                    "test2",
                                    "testAccounts",
                                    "testDataWithPlot",
                                    "testDataWithPlot",
                                    "test",
                                    "testFileData",
                                    "test01",
                                    "test03",
                                    "test04"};

    std::unique_ptr<DatasetSpreadsheet> createDataset(const QString& fileName);

    void addTestCaseForOdsAndXlsxComparison(const QString& testNamePrefix);

    void compareOdsAndXlsxExpectedData(const QString& fileSuffix);

    void checkDatasetDefinition(
        const std::unique_ptr<DatasetSpreadsheet>& dataset,
        const QString& expectedDefinitionFileName);

    void activateAllDatasetColumns(
        const std::unique_ptr<DatasetSpreadsheet>& dataset);

    void generateExpectedDataForFile(const QString& fileName,
                                     const QString& dir);

    void generateExpectedData();

    QString getSpreadsheetsDir();

    void saveExpectedDefinition(
        const std::unique_ptr<DatasetSpreadsheet>& dataset,
        const QString& filePath);

    void saveExpectedTsv(const QTableView& view, const QString& filePath);

    void addTestCasesForFileNames(const QVector<QString>& fileNames);
};
