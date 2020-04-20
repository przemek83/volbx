#ifndef SPREADSHEETSTEST_H
#define SPREADSHEETSTEST_H

#include <QObject>

#include "Datasets/DatasetDefinitionSpreadsheet.h"
#include "Datasets/DatasetSpreadsheet.h"

class DatasetDefinition;

/**
 * @brief Unit test for spreadsheets.
 */
class SpreadsheetsTest : public QObject
{
    Q_OBJECT
private:
    /**
     * @brief test few basic info of dataset.
     * @param definition definition to test.
     * @param rows number of rows to compare.
     * @param columns number of columns to compare.
     * @param activeColumns number of active columns.
     * @param name name of dataset to compare.
     */
    void testBasicInfo(DatasetDefinition& definition, int rows, int columns,
                       int activeColumns, QString name);

    /**
     * @brief check some columns if matches hardcoded values.
     * @param definition dataset definition.
     * @param columnFormats format of columns to compare.
     * @param columnNames names of columns to compare.
     */
    void testColumnInfo(DatasetDefinition& definition,
                        const QVector<QPair<int, ColumnType> >& columnFormats,
                        const QVector<QPair<int, QString> >& columnNames);

    /**
     * @brief check number of rows, columns and some values in data.
     * @param definition dataset definition to test.
     * @param rows number of rows.
     * @param columns number of columns.
     * @param fields values to compare with data in dataset.
     */
    void testSampleData(DatasetDefinition& definition, int rows, int columns,
                        const QVector<std::tuple<QVariant, int, int> >& fields);

    void generateDataDumpsForFile(QString name);

    void generateAllDumpData();

    QString getSpreadsheetsDir();

    void testSpreadsheetFile01(DatasetDefinitionSpreadsheet* definition,
                               QString file);

    void testSpreadsheetFile01SomeColumns(
        DatasetDefinitionSpreadsheet* definition);

    //    void testSpreadsheetFile02(DatasetDefinitionSpreadsheet* definition,
    //                               QString file);
    void testSpreadsheetFile03(DatasetDefinitionSpreadsheet* definition,
                               QString file);
    void testSpreadsheetFile04(DatasetDefinitionSpreadsheet* definition,
                               QString file);
    //    void testSpreadsheetFile05(DatasetDefinitionSpreadsheet* definition,
    //                               QString file);

    void compareDataWithDumps(const QString& category, bool damaged);

    void testDatasetConstruction(const Dataset& dataset,
                                 QVector<int>& columnsToTest,
                                 QVector<double>& compareNumericValues,
                                 QVector<QDate>& compareDateValues,
                                 QStringList& compareList, bool emptyDates);

    void compareExportDataWithDump(std::unique_ptr<Dataset> dataset);

    /// To be used when .ods and .xlsx has same data. Actualy data is not same
    /// :(
    void compareAllTsvDumps();

private slots:
    void initTestCase();

    /// Detailed test for 5 files.
    void detailedSpreadsheetFileTest01();
    // void detailedSpreadsheetFileTest02();
    void detailedSpreadsheetFileTest03();
    void detailedSpreadsheetFileTest04();
    // void detailedSpreadsheetFileTest05();

    /// Test for each category (directory).
    void verySmallFiles();
    void smallFiles();
    void mediumFiles();
    void bigFiles();
    void veryBigFiles();
    void damaged();

    void compareAllDefinitionDumps();

    void cleanupTestCase();
};

#endif  // SPREADSHEETSTEST_H
