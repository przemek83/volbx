#include "DetailedSpreadsheetsTest.h"

#include <QApplication>
#include <QtTest/QtTest>

#include <Common/Constants.h>
#include <Common/FileUtilities.h>
#include <Datasets/DatasetOds.h>
#include <Datasets/DatasetXlsx.h>

#include "Common.h"
#include "DatasetCommon.h"

void DetailedSpreadsheetsTest::testBasics_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<unsigned int>("rowCount");
    QTest::addColumn<unsigned int>("columnCount");

    const QVector<unsigned int> expectedRowCounts{49, 4, 30};
    const QVector<unsigned int> expectedColumnCounts{7, 5, 12};

    for (int i = 0; i < fileNames_.size(); ++i)
    {
        const QString testName{"Basic test for " + fileNames_[i] + "."};
        QTest::newRow(testName.toStdString().c_str())
            << fileNames_[i] << expectedRowCounts[i / 2]
            << expectedColumnCounts[i / 2];
    }
}

void DetailedSpreadsheetsTest::testBasics()
{
    QFETCH(const QString, fileName);
    QFETCH(const unsigned int, rowCount);
    QFETCH(const unsigned int, columnCount);

    const QString filePath(Common::getSpreadsheetsDir() + fileName);
    std::unique_ptr<Dataset> dataset{
        DatasetCommon::createDataset(fileName, filePath)};

    QVERIFY(dataset->initialize());
    QVERIFY(dataset->isValid());
    QCOMPARE(dataset->rowCount(), rowCount);
    QCOMPARE(dataset->columnCount(), columnCount);
    QCOMPARE(dataset->getName(), fileName);
}

void DetailedSpreadsheetsTest::testColumns_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<QVector<ColumnType>>("columnFormats");
    QTest::addColumn<QVector<QString>>("columnNames");

    for (int i = 0; i < fileNames_.size(); ++i)
    {
        const QString testName{"Column test for " + fileNames_[i] + "."};
        QTest::newRow(testName.toStdString().c_str())
            << fileNames_[i] << columnFormats_[i / 2] << columnNames_[i / 2];
    }
}

void DetailedSpreadsheetsTest::testColumns()
{
    QFETCH(const QString, fileName);
    QFETCH(const QVector<ColumnType>, columnFormats);
    QFETCH(const QVector<QString>, columnNames);

    const QString filePath(Common::getSpreadsheetsDir() + fileName);
    std::unique_ptr<Dataset> dataset{
        DatasetCommon::createDataset(fileName, filePath)};
    dataset->initialize();

    checkColumnFormats(dataset, columnFormats);
    checkColumnNames(dataset, columnNames);
}

Q_DECLARE_METATYPE(DetailedSpreadsheetsTest::Field)

void DetailedSpreadsheetsTest::testSampleData_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<unsigned int>("sampleRowCount");
    QTest::addColumn<unsigned int>("sampleColumnCount");
    QTest::addColumn<QVector<Field>>("sampleFields");

    const QVector<unsigned int> expectedSampleRowCounts{10, 4, 10};
    const QVector<unsigned int> expectedSampleColumnCounts{7, 5, 12};

    for (int i = 0; i < fileNames_.size(); ++i)
    {
        const QString testName{"Sample data test for " + fileNames_[i] + "."};
        QTest::newRow(testName.toStdString().c_str())
            << fileNames_[i] << expectedSampleRowCounts[i / 2]
            << expectedSampleColumnCounts[i / 2] << sampleFields_[i / 2];
    }
}

void DetailedSpreadsheetsTest::testSampleData()
{
    QFETCH(const QString, fileName);
    QFETCH(const unsigned int, sampleRowCount);
    QFETCH(const unsigned int, sampleColumnCount);
    QFETCH(const QVector<Field>, sampleFields);

    const QString filePath(Common::getSpreadsheetsDir() + fileName);
    std::unique_ptr<Dataset> dataset{
        DatasetCommon::createDataset(fileName, filePath)};
    dataset->initialize();

    QVector<QVector<QVariant>> sampleData{dataset->retrieveSampleData()};

    QCOMPARE(sampleData.size(), sampleRowCount);
    QCOMPARE(sampleData.front().size(), sampleColumnCount);

    for (const auto& [value, row, column] : sampleFields)
        QCOMPARE(sampleData.at(static_cast<int>(row))[static_cast<int>(column)],
                 value);
}

struct NumericCheckData
{
    int columnIndex{constants::NOT_SET_COLUMN};
    double min{0.};
    double max{0.};
};

Q_DECLARE_METATYPE(NumericCheckData)

void DetailedSpreadsheetsTest::testNumericColumnRanges_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<QVector<NumericCheckData>>("expectedRanges");

    const QVector<QVector<NumericCheckData>> expectedRanges{
        {{3, 19.23, 94.68}, {5, 3376.03, 23223.09}},
        {{0, 200000, 200003}, {1, 51, 54}},
        {{2, 0, 74.46}, {3, 0, 1.83}}};

    for (int i = 0; i < fileNames_.size(); ++i)
    {
        const QString testName{"Numeric ranges test for " + fileNames_[i] +
                               "."};
        QTest::newRow(testName.toStdString().c_str())
            << fileNames_[i] << expectedRanges[i / 2];
    }
}

void DetailedSpreadsheetsTest::testNumericColumnRanges()
{
    QFETCH(const QString, fileName);
    QFETCH(const QVector<NumericCheckData>, expectedRanges);

    const QString filePath(Common::getSpreadsheetsDir() + fileName);
    std::unique_ptr<Dataset> dataset{
        DatasetCommon::createDataset(fileName, filePath)};

    prepareDatasetForTest(dataset);

    for (const auto& [columnIndex, expectedMin, expectedMax] : expectedRanges)
        checkNumericColumnRange(dataset, columnIndex,
                                {expectedMin, expectedMax});
}

struct DateCheckData
{
    int columnIndex{constants::NOT_SET_COLUMN};
    QDate min{};
    QDate max{};
    bool emptyDates{false};
};

Q_DECLARE_METATYPE(DateCheckData)
Q_DECLARE_TYPEINFO(DateCheckData, Q_MOVABLE_TYPE);

void DetailedSpreadsheetsTest::testDateColumnRanges_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<DateCheckData>("expectedRanges");

    const QVector<DateCheckData> expectedRanges{
        {2, QDate(2010, 1, 7), QDate(2010, 2, 27), false},
        {3, QDate(2012, 2, 2), QDate(2012, 2, 5), true},
        {1, QDate(1970, 1, 1), QDate(1970, 1, 30), false}};

    for (int i = 0; i < fileNames_.size(); ++i)
    {
        const QString testName{"Numeric ranges test for " + fileNames_[i] +
                               "."};
        QTest::newRow(testName.toStdString().c_str())
            << fileNames_[i] << expectedRanges[i / 2];
    }
}

void DetailedSpreadsheetsTest::testDateColumnRanges()
{
    QFETCH(const QString, fileName);
    QFETCH(const DateCheckData, expectedRanges);

    const QString filePath(Common::getSpreadsheetsDir() + fileName);
    std::unique_ptr<Dataset> dataset{
        DatasetCommon::createDataset(fileName, filePath)};

    prepareDatasetForTest(dataset);

    const auto& [columnIndex, expectedMin, expectedMax, expectedEmptyDates] =
        expectedRanges;
    checkDateColumnRange(dataset, columnIndex, {expectedMin, expectedMax},
                         expectedEmptyDates);
}

void DetailedSpreadsheetsTest::testStringColumnRanges_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<int>("columnIndex");
    QTest::addColumn<QStringList>("expectedStrings");

    const QVector<int> columnIndexes{6, 4, 0};
    const QVector<QStringList> expectedStrings{
        {"brown", "red", "yellow", "black", "blue", "pink", "white"},
        {"a", "b", "c"},
        {}};

    for (int i = 0; i < fileNames_.size(); ++i)
    {
        const QString testName{"Numeric ranges test for " + fileNames_[i] +
                               "."};
        QTest::newRow(testName.toStdString().c_str())
            << fileNames_[i] << columnIndexes[i / 2] << expectedStrings[i / 2];
    }
}

void DetailedSpreadsheetsTest::testStringColumnRanges()
{
    QFETCH(const QString, fileName);
    QFETCH(const int, columnIndex);
    QFETCH(const QStringList, expectedStrings);

    const QString filePath(Common::getSpreadsheetsDir() + fileName);
    std::unique_ptr<Dataset> dataset{
        DatasetCommon::createDataset(fileName, filePath)};

    prepareDatasetForTest(dataset);

    checkStringColumnRange(dataset, columnIndex, expectedStrings);
}

void DetailedSpreadsheetsTest::testDataFile01SomeColumnsActive_data()
{
    QTest::addColumn<QString>("fileName");
    const QVector<QString> extensions{"xlsx", "ods"};
    for (const auto& extension : extensions)
    {
        const QString testName{"Detailed test for test01Partial " + extension};
        QTest::newRow(testName.toStdString().c_str())
            << "test01Partial." + extension;
    }
}

void DetailedSpreadsheetsTest::testDataFile01SomeColumnsActive()
{
    QFETCH(const QString, fileName);
    const QString filePath(Common::getSpreadsheetsDir() + fileName);
    std::unique_ptr<Dataset> dataset{
        DatasetCommon::createDataset(fileName, filePath)};

    dataset->initialize();
    QVector<bool> activeColumns(static_cast<int>(dataset->columnCount()), true);
    activeColumns[0] = false;
    activeColumns[1] = false;
    activeColumns[4] = false;
    dataset->setActiveColumns(activeColumns);
    dataset->loadData();

    checkNumericColumnRange(dataset, 1, {19.23, 94.68});
    checkNumericColumnRange(dataset, 2, {3376.03, 23223.09});

    checkDateColumnRange(dataset, 0, {QDate(2010, 1, 7), QDate(2010, 2, 27)},
                         false);

    const QStringList compareList = {"brown", "red",  "yellow", "black",
                                     "blue",  "pink", "white"};
    checkStringColumnRange(dataset, 3, compareList);

    DatasetCommon::compareExportDataWithDump(std::move(dataset), filePath);
}

void DetailedSpreadsheetsTest::checkColumnFormats(
    const std::unique_ptr<Dataset>& dataset,
    const QVector<ColumnType>& columnFormats)
{
    int column{0};
    for (const auto& expectedColumnType : columnFormats)
        QCOMPARE(dataset->getColumnFormat(column++), expectedColumnType);
}

void DetailedSpreadsheetsTest::checkColumnNames(
    const std::unique_ptr<Dataset>& dataset,
    const QVector<QString>& columnNames)
{
    int column{0};
    for (const auto& expectedColumnName : columnNames)
        QCOMPARE(dataset->getHeaderName(column++), expectedColumnName);
}

void DetailedSpreadsheetsTest::checkNumericColumnRange(
    const std::unique_ptr<Dataset>& dataset, int columnIndex,
    std::pair<double, double> expectedRange)
{
    auto [currentMin, currentMax] = dataset->getNumericRange(columnIndex);
    const auto [expectedMin, expectedMax] = expectedRange;
    QCOMPARE(currentMin, expectedMin);
    QCOMPARE(currentMax, expectedMax);
}

void DetailedSpreadsheetsTest::checkDateColumnRange(
    const std::unique_ptr<Dataset>& dataset, int columnIndex,
    std::pair<QDate, QDate> expectedRange, bool expectedEmptyDates)
{
    auto [currentMinDate, currentMaxDate, currentEmpty] =
        dataset->getDateRange(columnIndex);
    const auto [expectedMin, expectedMax] = expectedRange;
    QCOMPARE(currentMinDate, expectedMin);
    QCOMPARE(currentMaxDate, expectedMax);
    QCOMPARE(currentEmpty, expectedEmptyDates);
}

void DetailedSpreadsheetsTest::checkStringColumnRange(
    const std::unique_ptr<Dataset>& dataset, int columnIndex,
    const QStringList& expectedList)
{
    const QStringList currentList{dataset->getStringList(columnIndex)};
    QCOMPARE(currentList, expectedList);
}

void DetailedSpreadsheetsTest::prepareDatasetForTest(
    std::unique_ptr<Dataset>& dataset)
{
    dataset->initialize();
    DatasetCommon::activateAllDatasetColumns(*dataset);
    dataset->loadData();
}
