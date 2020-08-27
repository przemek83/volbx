#pragma once

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

    void testDataFile01_data();
    void testDataFile01();

    void testDataFile03_data();
    void testDataFile03();

    void testDataFile04_data();
    void testDataFile04();

private:
    void checkNumericColumnRange(
        const std::unique_ptr<DatasetSpreadsheet>& dataset, int columnIndex,
        double expectedMin, double expectedMax);

    void checkDateColumnRange(
        const std::unique_ptr<DatasetSpreadsheet>& dataset, int columnIndex,
        QDate expectedMin, QDate expectedMax, bool expectedEmptyDates);

    void checkStringColumnRange(
        const std::unique_ptr<DatasetSpreadsheet>& dataset, int columnIndex,
        QStringList& expectedList);

    void checkDataFile01(std::unique_ptr<DatasetSpreadsheet> dataset);

    void checkDataFile01SomeColumns(
        std::unique_ptr<DatasetSpreadsheet> dataset);

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
