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
    static void testDefinition();

    void testData_data();
    static void testData();

    static void testDamagedFiles_data();
    static void testDamagedFiles();

    void compareExpectedDefinitionsOfOdsAndXlsx_data();
    static void compareExpectedDefinitionsOfOdsAndXlsx();

    void compareExpectedTsvDumpsOfOdsAndXlsx_data();
    static void compareExpectedTsvDumpsOfOdsAndXlsx();

private:
    void addTestCaseForOdsAndXlsxComparison(const QString& testNamePrefix);

    static void compareOdsAndXlsxExpectedData(const QString& fileSuffix);

    void activateAllDatasetColumns(
        const std::unique_ptr<DatasetSpreadsheet>& dataset);

    void generateExpectedData();

    static void addTestCasesForFileNames(const QVector<QString>& fileNames);

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
