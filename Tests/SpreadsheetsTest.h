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

    void generateDataDumpsForFile(QString name);

    void generateAllDumpData();

    QString getSpreadsheetsDir();

    /// To be used when .ods and .xlsx has same data. Actually data is not same
    /// :(
    void compareAllTsvDumps();

    void addTestCasesForFileNames(const QVector<QString>& fileNames);
};
