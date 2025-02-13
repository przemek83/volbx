#pragma once

#include <eible/ColumnType.h>
#include <QDate>
#include <QObject>
#include <QString>
#include <QVariant>
#include <QVector>

class DatasetSpreadsheet;
class Dataset;

class DetailedSpreadsheetsTest : public QObject
{
    Q_OBJECT
public:
    struct Field
    {
        QVariant data;
        int row{0};
        int column{0};
    };

private slots:
    void testBasics_data();
    static void testBasics();

    void testColumns_data() const;
    static void testColumns();

    void testSampleData_data() const;
    static void testSampleData();

    void testNumericColumnRanges_data() const;
    static void testNumericColumnRanges();

    void testDateColumnRanges_data() const;
    static void testDateColumnRanges();

    void testStringColumnRanges_data() const;
    static void testStringColumnRanges();

    static void testDataFile01SomeColumnsActive_data();
    static void testDataFile01SomeColumnsActive();

private:
    static void checkNumericColumnRange(
        const Dataset& dataset, int columnIndex,
        std::pair<double, double> expectedRange);

    static void checkDateColumnRange(const Dataset& dataset, int columnIndex,
                                     std::pair<QDate, QDate> expectedRange,
                                     bool expectedEmptyDates);

    static void checkStringColumnRange(const Dataset& dataset, int columnIndex,
                                       const QStringList& expectedList);

    static void checkColumnFormats(const Dataset& dataset,
                                   const QVector<ColumnType>& columnFormats);

    static void checkColumnNames(const Dataset& dataset,
                                 const QVector<QString>& columnNames);

    static void prepareDatasetForTest(Dataset& dataset);

    const QVector<QString> fileNames_{"test01.xlsx", "test01.ods",
                                      "test03.xlsx", "test03.ods",
                                      "test04.xlsx", "test04.ods"};

    const QVector<QVector<ColumnType>> columnFormats_{
        {ColumnType::STRING, ColumnType::NUMBER, ColumnType::DATE,
         ColumnType::NUMBER, ColumnType::NUMBER, ColumnType::NUMBER,
         ColumnType::STRING},
        {ColumnType::NUMBER, ColumnType::NUMBER, ColumnType::NUMBER,
         ColumnType::DATE, ColumnType::STRING},
        {ColumnType::STRING, ColumnType::DATE, ColumnType::NUMBER,
         ColumnType::NUMBER, ColumnType::DATE, ColumnType::NUMBER,
         ColumnType::STRING, ColumnType::STRING, ColumnType::NUMBER,
         ColumnType::NUMBER, ColumnType::NUMBER, ColumnType::NUMBER}};

    const QVector<QVector<QString>> columnNames_{
        {"Trait #1", "Value #1", "Transaction date", "Units", "Price",
         "Price per unit", "Another trait"},
        {"cena nier", "pow", "cena metra", "data transakcji", "text"},
        {"name", "date", "mass (kg)", "height", "no name", "no name", "no name",
         "no name", "no name", "no name", "no name", "no name"}};

    const QVector<QVector<Field>> sampleFields_{
        {{QVariant(QDate::fromJulianDay(2455207)), 3, 2},
         {QVariant("black"), 3, 6},
         {QVariant(12.0), 5, 1},
         {QVariant(4462.2), 3, 5}},
        {{QVariant(3703.75925925926), 3, 2},
         {QVariant(53.0), 2, 1},
         {QVariant(3773.62264150943), 2, 2}},
        {{QVariant(1.55), 3, 3}, {QVariant(58.57), 5, 2}}};
};
