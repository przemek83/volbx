#ifndef DETAILEDSPREADSHEETSTEST_H
#define DETAILEDSPREADSHEETSTEST_H

#include <memory>

#include <QObject>
#include <QString>
#include <QVariant>
#include <QVector>

#include <ColumnType.h>

class DatasetSpreadsheet;
class Dataset;
struct Field;

class DetailedSpreadsheetsTest : public QObject
{
    Q_OBJECT
private slots:
    void testBasics_data();
    void testBasics();

    void testColumns_data();
    void testColumns();

    void testSampleData_data();
    void testSampleData();

    void testDetailedSpreadsheetFile01_data();
    void testDetailedSpreadsheetFile01();

    // void detailedSpreadsheetFileTest02();

    void testDetailedSpreadsheetFile03_data();
    void testDetailedSpreadsheetFile03();

    void testDetailedSpreadsheetFile04_data();
    void testDetailedSpreadsheetFile04();

    // void detailedSpreadsheetFileTest05();

private:
    QString getSpreadsheetsDir();

    void testDatasetConstruction(const Dataset& dataset,
                                 QVector<int>& columnsToTest,
                                 QVector<double>& compareNumericValues,
                                 QVector<QDate>& compareDateValues,
                                 QStringList& compareList, bool emptyDates);

    void testSpreadsheetFile01(std::unique_ptr<DatasetSpreadsheet> dataset,
                               QString file);

    void testSpreadsheetFile01SomeColumns(
        std::unique_ptr<DatasetSpreadsheet> dataset);

    //    void testSpreadsheetFile02(DatasetDefinitionSpreadsheet* definition,
    //                               QString file);

    void testSpreadsheetFile04(std::unique_ptr<DatasetSpreadsheet> dataset,
                               QString file);

    //    void testSpreadsheetFile05(DatasetDefinitionSpreadsheet* definition,
    //                               QString file);

    void checkColumnFormats(const std::unique_ptr<DatasetSpreadsheet>& dataset,
                            const QVector<ColumnType>& columnFormats);

    void checkColumnNames(const std::unique_ptr<DatasetSpreadsheet>& dataset,
                          const QVector<QString>& columnNames);

    void checkTaggedColumnsNotSet(
        const std::unique_ptr<DatasetSpreadsheet>& dataset);

    static const QVector<QString> fileNames_;
    const QVector<QString> extensions_{"xlsx", "ods"};
    static const QVector<QVector<ColumnType>> columnFormats_;
    static const QVector<QVector<QString>> columnNames_;
    static const QVector<QVector<Field>> sampleFields_;
};

#endif  // DETAILEDSPREADSHEETSTEST_H
