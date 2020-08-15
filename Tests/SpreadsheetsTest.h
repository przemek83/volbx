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

    void compareSpreadsheetFiles_data();
    void compareSpreadsheetFiles();

    void compareSpreadsheetFilesDamaged_data();
    void compareSpreadsheetFilesDamaged();

    void compareAllDefinitionDumps();

private:
    void generateDataDumpsForFile(QString name);

    void generateAllDumpData();

    QString getSpreadsheetsDir();

    /// To be used when .ods and .xlsx has same data. Actually data is not same
    /// :(
    void compareAllTsvDumps();

    void addTestCasesForFileNames(const QVector<QString>& fileNames);
};
