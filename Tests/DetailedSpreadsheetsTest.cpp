#include "DetailedSpreadsheetsTest.h"

#include <QApplication>
#include <QtTest/QtTest>

#include <DatasetOds.h>
#include <DatasetXlsx.h>
#include <FileUtilities.h>

#include "Common.h"

void DetailedSpreadsheetsTest::performBasicChecks(
    const Dataset& dataset, int expectedRowCount, int expectedColumnCount,
    const QString& expectedDatasetName)
{
    QVERIFY(dataset.isValid());
    QCOMPARE(dataset.rowCount(), expectedRowCount);
    QCOMPARE(dataset.columnCount(), expectedColumnCount);
    QCOMPARE(dataset.getName(), expectedDatasetName);
}

void DetailedSpreadsheetsTest::testColumnInfo(
    Dataset& dataset, const QVector<QPair<int, ColumnType>>& columnFormats,
    const QVector<QPair<int, QString>>& columnNames)
{
    for (auto& [column, expectedColumnType] : columnFormats)
        QCOMPARE(dataset.getColumnFormat(column), expectedColumnType);

    // No tagged columns yet.
    auto [ok, column] = dataset.getTaggedColumn(ColumnTag::DATE);
    QVERIFY(!ok);
    std::tie(ok, column) = dataset.getTaggedColumn(ColumnTag::VALUE);
    QVERIFY(!ok);

    for (auto& [column, expectedColumnName] : columnNames)
        QCOMPARE(dataset.getHeaderName(column), expectedColumnName);
}

void DetailedSpreadsheetsTest::testSampleData(
    Dataset& dataset, int rows, int columns,
    const QVector<std::tuple<QVariant, int, int>>& fields)
{
    QVector<QVector<QVariant>> sampleData{dataset.retrieveSampleData()};

    QCOMPARE(sampleData.size(), rows);
    QCOMPARE(sampleData.front().size(), columns);

    std::tuple<QVariant, int, int> fieldsTuple;
    for (auto [value, row, column] : fields)
        QCOMPARE(sampleData.at(row)[column], value);
}

QString DetailedSpreadsheetsTest::getSpreadsheetsDir()
{
    return QString(":/TestFiles/TestSpreadsheets/");
}

void DetailedSpreadsheetsTest::testSpreadsheetFile01(
    std::unique_ptr<DatasetSpreadsheet> dataset, QString file)
{
    QVERIFY(dataset->initialize());
    performBasicChecks(*dataset, 4000, 7, file);

    QVector<QPair<int, ColumnType>> columnFormats;
    columnFormats.append(qMakePair(0, ColumnType::STRING));
    columnFormats.append(qMakePair(3, ColumnType::NUMBER));
    columnFormats.append(qMakePair(5, ColumnType::NUMBER));
    QVector<QPair<int, QString>> columnNames;
    columnNames.append(qMakePair(0, QString("Trait #1")));
    columnNames.append(qMakePair(3, QString("Units")));
    columnNames.append(qMakePair(5, QString("Price per unit")));
    testColumnInfo(*dataset, columnFormats, columnNames);

    QVector<std::tuple<QVariant, int, int>> fields;
    fields.append(
        std::make_tuple(QVariant(QDate::fromJulianDay(2455207)), 3, 2));
    fields.append(std::make_tuple(QVariant("black"), 3, 6));
    fields.append(std::make_tuple(QVariant(12.0), 5, 1));
    fields.append(std::make_tuple(QVariant(4462.2), 3, 5));
    testSampleData(*dataset, 10, 7, fields);

    QVector<bool> activeColumns(dataset->columnCount(), true);
    dataset->setActiveColumns(activeColumns);

    QVector<int> columnsToTest;
    columnsToTest << 5 << 3 << 2 << 6 << 2 << 5;

    QVector<double> compareNumericValues;
    compareNumericValues << 803.25 << 39999.98 << 14.91 << 126.69;

    QVector<QDate> compareDateValues;
    compareDateValues << QDate(2010, 1, 1) << QDate(2011, 8, 31);

    QStringList compareList;
    compareList << "brown"
                << "red"
                << "yellow"
                << "black"
                << "blue"
                << "pink"
                << "white"
                << "dark blue"
                << "orange";

    dataset->loadData();

    // Tagged column after rebuild done in dataset init.
    dataset->setTaggedColumn(ColumnTag::DATE, 2);
    dataset->setTaggedColumn(ColumnTag::VALUE, 5);

    testDatasetConstruction(*dataset, columnsToTest, compareNumericValues,
                            compareDateValues, compareList, false);
}

void DetailedSpreadsheetsTest::testSpreadsheetFile01SomeColumns(
    std::unique_ptr<DatasetSpreadsheet> dataset)
{
    QVERIFY(dataset->initialize());
    QVector<bool> activeColumns(dataset->columnCount(), true);
    activeColumns[0] = false;
    activeColumns[1] = false;
    activeColumns[4] = false;
    dataset->setActiveColumns(activeColumns);

    QVector<int> columnsToTest;
    columnsToTest << 2 << 1 << 0 << 3 << 0 << 2;

    QVector<double> compareNumericValues;
    compareNumericValues << 803.25 << 39999.98 << 14.91 << 126.69;

    QVector<QDate> compareDateValues;
    compareDateValues << QDate(2010, 1, 1) << QDate(2011, 8, 31);

    QStringList compareList;
    compareList << "brown"
                << "red"
                << "yellow"
                << "black"
                << "blue"
                << "pink"
                << "white"
                << "dark blue"
                << "orange";

    dataset->loadData();
    dataset->setTaggedColumn(ColumnTag::DATE, 0);
    dataset->setTaggedColumn(ColumnTag::VALUE, 2);

    testDatasetConstruction(*dataset, columnsToTest, compareNumericValues,
                            compareDateValues, compareList, false);

    Common::compareExportDataWithDump(std::move(dataset));  //"partial"
}

void DetailedSpreadsheetsTest::testDetailedSpreadsheetFile01_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<QString>("partialFileName");
    for (const auto& extension : QVector<QString>{"xlsx", "ods"})
    {
        QString testName{"Detailed test for test01 " + extension};
        QTest::newRow(testName.toStdString().c_str())
            << "test01." + extension << "test01Partial." + extension;
    }
}

void DetailedSpreadsheetsTest::testDetailedSpreadsheetFile01()
{
    QFETCH(QString, fileName);
    QFETCH(QString, partialFileName);

    QString filePath(getSpreadsheetsDir() + fileName);
    std::unique_ptr<DatasetSpreadsheet> dataset{
        Common::createDataset(filePath)};
    testSpreadsheetFile01(std::move(dataset), filePath);
    filePath = getSpreadsheetsDir() + partialFileName;
    dataset = Common::createDataset(filePath);
    testSpreadsheetFile01SomeColumns(std::move(dataset));
}

// void SpreadsheetsTest::detailedSpreadsheetFileTest02()
//{
//    QString file(getSpreadsheetsDir() + "test02.xlsx");
//    DatasetXlsx* definitionXlsx = new DatasetXlsx(file,
//    file); testSpreadsheetFile02(definitionXlsx, file);

//    file = getSpreadsheetsDir() + "test02.ods";
//    DatasetOds* definitionOds = new DatasetOds(file,
//    file); testSpreadsheetFile02(definitionOds, file);
//}

void DetailedSpreadsheetsTest::testDetailedSpreadsheetFile03_data()
{
    QTest::addColumn<QString>("fileName");
    for (const auto& extension : QVector<QString>{"xlsx", "ods"})
    {
        QString testName{"Detailed test for test03 " + extension};
        QTest::newRow(testName.toStdString().c_str()) << "test03." + extension;
    }
}

void DetailedSpreadsheetsTest::testDetailedSpreadsheetFile03()
{
    QFETCH(QString, fileName);

    QString filePath(getSpreadsheetsDir() + fileName);
    std::unique_ptr<DatasetSpreadsheet> dataset{
        Common::createDataset(filePath)};
    testSpreadsheetFile03(std::move(dataset), filePath);
}

void DetailedSpreadsheetsTest::testDetailedSpreadsheetFile04_data()
{
    QTest::addColumn<QString>("fileName");
    for (const auto& extension : QVector<QString>{"xlsx", "ods"})
    {
        QString testName{"Detailed test for test04 " + extension};
        QTest::newRow(testName.toStdString().c_str()) << "test04." + extension;
    }
}

void DetailedSpreadsheetsTest::testDetailedSpreadsheetFile04()
{
    QFETCH(QString, fileName);

    QString filePath(getSpreadsheetsDir() + fileName);
    std::unique_ptr<DatasetSpreadsheet> dataset{
        Common::createDataset(filePath)};
    testSpreadsheetFile04(std::move(dataset), filePath);
}

// void
// DetailedSpreadsheetsTest::testSpreadsheetFile02(DatasetDefinitionSpreadsheet*
// definition,
//                                         QString file)
//{
//    QVERIFY(true == definition->init());
//}

void DetailedSpreadsheetsTest::testSpreadsheetFile03(
    std::unique_ptr<DatasetSpreadsheet> dataset, QString file)
{
    QVERIFY(dataset->initialize());
    performBasicChecks(*dataset, 4, 5, file);

    QVector<QPair<int, ColumnType>> columnFormats;
    columnFormats.append(qMakePair(0, ColumnType::NUMBER));
    columnFormats.append(qMakePair(1, ColumnType::NUMBER));
    columnFormats.append(qMakePair(2, ColumnType::NUMBER));
    QVector<QPair<int, QString>> columnNames;
    columnNames.append(qMakePair(0, QString("cena nier")));
    columnNames.append(qMakePair(1, QString("pow")));
    columnNames.append(qMakePair(3, QString("data transakcji")));
    testColumnInfo(*dataset, columnFormats, columnNames);

    QVector<std::tuple<QVariant, int, int>> fields;
    fields.append(std::make_tuple(QVariant(3703.75925925926), 3, 2));
    fields.append(std::make_tuple(QVariant(53.0), 2, 1));
    fields.append(std::make_tuple(QVariant(3773.62264150943), 2, 2));
    testSampleData(*dataset, 4, 5, fields);

    QVector<bool> activeColumns(dataset->columnCount(), true);
    dataset->setActiveColumns(activeColumns);

    QVector<int> columnsToTest;
    columnsToTest << 0 << 1 << 3 << 4 << 3 << 2;

    QVector<double> compareNumericValues;
    compareNumericValues << 200000 << 200003 << 51 << 54;

    QVector<QDate> compareDateValues;
    compareDateValues << QDate(2012, 2, 2) << QDate(2012, 2, 5);

    QStringList compareList;
    compareList << "a"
                << "b"
                << "c";

    dataset->loadData();
    dataset->setTaggedColumn(ColumnTag::DATE, 3);
    dataset->setTaggedColumn(ColumnTag::VALUE, 2);

    testDatasetConstruction(*dataset, columnsToTest, compareNumericValues,
                            compareDateValues, compareList, true);
}

void DetailedSpreadsheetsTest::testSpreadsheetFile04(
    std::unique_ptr<DatasetSpreadsheet> dataset, QString file)
{
    QVERIFY(dataset->initialize());
    performBasicChecks(*dataset, 30, 12, file);

    QVector<QPair<int, ColumnType>> columnFormats;
    columnFormats.append(qMakePair(0, ColumnType::STRING));
    columnFormats.append(qMakePair(1, ColumnType::DATE));
    columnFormats.append(qMakePair(2, ColumnType::NUMBER));
    columnFormats.append(qMakePair(3, ColumnType::NUMBER));
    columnFormats.append(qMakePair(4, ColumnType::DATE));
    columnFormats.append(qMakePair(5, ColumnType::NUMBER));
    columnFormats.append(qMakePair(6, ColumnType::STRING));
    columnFormats.append(qMakePair(7, ColumnType::STRING));
    columnFormats.append(qMakePair(8, ColumnType::NUMBER));
    columnFormats.append(qMakePair(9, ColumnType::NUMBER));
    columnFormats.append(qMakePair(10, ColumnType::NUMBER));
    columnFormats.append(qMakePair(11, ColumnType::NUMBER));

    QVector<QPair<int, QString>> columnNames;
    columnNames.append(qMakePair(0, QString("name")));
    columnNames.append(qMakePair(1, QString("date")));
    columnNames.append(qMakePair(2, QString("mass (kg)")));
    columnNames.append(qMakePair(3, QString("height")));
    columnNames.append(qMakePair(4, QString("no name")));
    columnNames.append(qMakePair(5, QString("no name")));
    columnNames.append(qMakePair(6, QString("no name")));
    columnNames.append(qMakePair(7, QString("no name")));
    columnNames.append(qMakePair(8, QString("no name")));
    columnNames.append(qMakePair(9, QString("no name")));
    columnNames.append(qMakePair(10, QString("no name")));
    columnNames.append(qMakePair(11, QString("no name")));
    testColumnInfo(*dataset, columnFormats, columnNames);

    QVector<std::tuple<QVariant, int, int>> fields;
    fields.append(std::make_tuple(QVariant(1.55), 3, 3));
    fields.append(std::make_tuple(QVariant(58.57), 5, 2));
    testSampleData(*dataset, 10, 12, fields);

    QVector<bool> activeColumns(dataset->columnCount(), true);
    dataset->setActiveColumns(activeColumns);

    QVector<int> columnsToTest;
    columnsToTest << 2 << 3 << 1 << 0 << 1 << 2;

    QVector<double> compareNumericValues;
    compareNumericValues << 0 << 74.46 << 0 << 1.83;

    QVector<QDate> compareDateValues;
    compareDateValues << QDate(1970, 1, 1) << QDate(1970, 1, 30);

    QStringList compareList;

    dataset->loadData();
    dataset->setTaggedColumn(ColumnTag::DATE, 1);
    dataset->setTaggedColumn(ColumnTag::VALUE, 2);

    testDatasetConstruction(*dataset, columnsToTest, compareNumericValues,
                            compareDateValues, compareList, false);
}

// void
// DetailedSpreadsheetsTest::testSpreadsheetFile05(DatasetDefinitionSpreadsheet*
// definition,
//                                             QString file)
//{
//    QVERIFY(true == definition->init());
//}

void DetailedSpreadsheetsTest::testDatasetConstruction(
    const Dataset& dataset, QVector<int>& columnsToTest,
    QVector<double>& compareNumericValues, QVector<QDate>& compareDateValues,
    QStringList& compareList, bool emptyDates)
{
    QVERIFY(dataset.isValid());

    auto [min, max] = dataset.getNumericRange(columnsToTest[0]);
    QCOMPARE(min, compareNumericValues[0]);
    QCOMPARE(max, compareNumericValues[1]);

    std::tie(min, max) = dataset.getNumericRange(columnsToTest[1]);
    QCOMPARE(min, compareNumericValues[2]);
    QCOMPARE(max, compareNumericValues[3]);

    auto [minDate, maxDate, empty] = dataset.getDateRange(columnsToTest[2]);
    QCOMPARE(minDate, compareDateValues[0]);
    QCOMPARE(maxDate, compareDateValues[1]);
    QCOMPARE(empty, emptyDates);

    QStringList list = dataset.getStringList(columnsToTest[3]);
    QCOMPARE(list, compareList);

    auto [ok, column] = dataset.getTaggedColumn(ColumnTag::DATE);
    QCOMPARE(column, columnsToTest[4]);
    std::tie(ok, column) = dataset.getTaggedColumn(ColumnTag::VALUE);
    QCOMPARE(column, columnsToTest[5]);
}
