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

    void generateDataDumpsForFile(QString name);

    void generateAllDumpData();

    QString getSpreadsheetsDir();

    void addTestCasesForFileNames(const QVector<QString>& fileNames);
};
