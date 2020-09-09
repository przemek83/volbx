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
private Q_SLOTS:
    void initTestCase();

    void testDefinition_data();
    void testDefinition();

    void testData_data();
    void testData();

    void testDamagedFiles_data();
    void testDamagedFiles();

    void compareExpectedDefinitionsOfOdsAndXlsx_data();
    void compareExpectedDefinitionsOfOdsAndXlsx();

    void compareExpectedTsvDumpsOfOdsAndXlsx_data();
    void compareExpectedTsvDumpsOfOdsAndXlsx();

private:
    void addTestCaseForOdsAndXlsxComparison(const QString& testNamePrefix);

    void compareOdsAndXlsxExpectedData(const QString& fileSuffix);

    void activateAllDatasetColumns(
        const std::unique_ptr<DatasetSpreadsheet>& dataset);

    void generateExpectedData();

    void addTestCasesForFileNames(const QVector<QString>& fileNames);

    const QVector<QString> testFileNames_{"excel",
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
};
