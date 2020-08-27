#include "DetailedSpreadsheetsTest.h"

#include <QApplication>
#include <QtTest/QtTest>

#include <DatasetOds.h>
#include <DatasetXlsx.h>
#include <FileUtilities.h>

#include "Common.h"

const QVector<QString> DetailedSpreadsheetsTest::fileNames_{"test01", "test03",
                                                            "test04"};

const QVector<QVector<ColumnType>> DetailedSpreadsheetsTest::columnFormats_{
    {ColumnType::STRING, ColumnType::NUMBER, ColumnType::DATE,
     ColumnType::NUMBER, ColumnType::NUMBER, ColumnType::NUMBER,
     ColumnType::STRING},
    {ColumnType::NUMBER, ColumnType::NUMBER, ColumnType::NUMBER,
     ColumnType::DATE, ColumnType::STRING},
    {ColumnType::STRING, ColumnType::DATE, ColumnType::NUMBER,
     ColumnType::NUMBER, ColumnType::DATE, ColumnType::NUMBER,
     ColumnType::STRING, ColumnType::STRING, ColumnType::NUMBER,
     ColumnType::NUMBER, ColumnType::NUMBER, ColumnType::NUMBER}};

const QVector<QVector<QString>> DetailedSpreadsheetsTest::columnNames_{
    {"Trait #1", "Value #1", "Transaction date", "Units", "Price",
     "Price per unit", "Another trait"},
    {"cena nier", "pow", "cena metra", "data transakcji", "text"},
    {"name", "date", "mass (kg)", "height", "no name", "no name", "no name",
     "no name", "no name", "no name", "no name", "no name"}};

struct Field
{
    QVariant data;
    unsigned int row;
    unsigned int column;
};

Q_DECLARE_METATYPE(Field)

const QVector<QVector<Field>> DetailedSpreadsheetsTest::sampleFields_{
    {{QVariant(QDate::fromJulianDay(2455207)), 3, 2},
     {QVariant("black"), 3, 6},
     {QVariant(12.0), 5, 1},
     {QVariant(4462.2), 3, 5}},
    {{QVariant(3703.75925925926), 3, 2},
     {QVariant(53.0), 2, 1},
     {QVariant(3773.62264150943), 2, 2}},
    {{QVariant(1.55), 3, 3}, {QVariant(58.57), 5, 2}}};

void DetailedSpreadsheetsTest::testBasics_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<unsigned int>("rowCount");
    QTest::addColumn<unsigned int>("columnCount");

    const QVector<unsigned int> expectedRowCounts{4000, 4, 30};
    const QVector<unsigned int> expectedColumnCounts{7, 5, 12};

    for (int i = 0; i < fileNames_.size(); ++i)
        for (const auto& extension : extensions_)
        {
            QString testName{"Basic test for " + fileNames_[i] + " " +
                             extension};
            QTest::newRow(testName.toStdString().c_str())
                << fileNames_[i] + "." + extension << expectedRowCounts[i]
                << expectedColumnCounts[i];
        }
}

void DetailedSpreadsheetsTest::testBasics()
{
    QFETCH(QString, fileName);
    QFETCH(unsigned int, rowCount);
    QFETCH(unsigned int, columnCount);

    QString filePath(Common::getSpreadsheetsDir() + fileName);
    std::unique_ptr<DatasetSpreadsheet> dataset{
        Common::createDataset(filePath)};

    QVERIFY(dataset->initialize());
    QVERIFY(dataset->isValid());
    QCOMPARE(dataset->rowCount(), rowCount);
    QCOMPARE(dataset->columnCount(), columnCount);
    QCOMPARE(dataset->getName(), filePath);
}

void DetailedSpreadsheetsTest::testColumns_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<QVector<ColumnType>>("columnFormats");
    QTest::addColumn<QVector<QString>>("columnNames");

    for (int i = 0; i < fileNames_.size(); ++i)
        for (const auto& extension : extensions_)
        {
            QString testName{"Column test for " + fileNames_[i] + " " +
                             extension};
            QTest::newRow(testName.toStdString().c_str())
                << fileNames_[i] + "." + extension << columnFormats_[i]
                << columnNames_[i];
        }
}

void DetailedSpreadsheetsTest::testColumns()
{
    QFETCH(QString, fileName);
    QFETCH(QVector<ColumnType>, columnFormats);
    QFETCH(QVector<QString>, columnNames);

    QString filePath(Common::getSpreadsheetsDir() + fileName);
    std::unique_ptr<DatasetSpreadsheet> dataset{
        Common::createDataset(filePath)};
    dataset->initialize();

    checkColumnFormats(dataset, columnFormats);
    checkColumnNames(dataset, columnNames);
}

void DetailedSpreadsheetsTest::testSampleData_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<unsigned int>("sampleRowCount");
    QTest::addColumn<unsigned int>("sampleColumnCount");
    QTest::addColumn<QVector<Field>>("sampleFields");

    const QVector<unsigned int> expectedSampleRowCounts{10, 4, 10};
    const QVector<unsigned int> expectedSampleColumnCounts{7, 5, 12};

    for (int i = 0; i < fileNames_.size(); ++i)
        for (const auto& extension : extensions_)
        {
            QString testName{"Sample data test for " + fileNames_[i] + " " +
                             extension};
            QTest::newRow(testName.toStdString().c_str())
                << fileNames_[i] + "." + extension << expectedSampleRowCounts[i]
                << expectedSampleColumnCounts[i] << sampleFields_[i];
        }
}

void DetailedSpreadsheetsTest::testSampleData()
{
    QFETCH(QString, fileName);
    QFETCH(unsigned int, sampleRowCount);
    QFETCH(unsigned int, sampleColumnCount);
    QFETCH(QVector<Field>, sampleFields);

    QString filePath(Common::getSpreadsheetsDir() + fileName);
    std::unique_ptr<DatasetSpreadsheet> dataset{
        Common::createDataset(filePath)};
    dataset->initialize();

    QVector<QVector<QVariant>> sampleData{dataset->retrieveSampleData()};

    QCOMPARE(sampleData.size(), sampleRowCount);
    QCOMPARE(sampleData.front().size(), sampleColumnCount);

    for (auto [value, row, column] : sampleFields)
        QCOMPARE(sampleData.at(row)[column], value);
}

struct NumericCheckData
{
    unsigned int columnIndex;
    double min;
    double max;
};

Q_DECLARE_METATYPE(NumericCheckData)

void DetailedSpreadsheetsTest::testNumericColumnRanges_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<QVector<NumericCheckData>>("expectedRanges");

    const QVector<QVector<NumericCheckData>> expectedRanges{
        {{3, 14.91, 126.69}, {5, 803.25, 39999.98}},
        {{0, 200000, 200003}, {1, 51, 54}},
        {{2, 0, 74.46}, {3, 0, 1.83}}};

    for (int i = 0; i < fileNames_.size(); ++i)
        for (const auto& extension : extensions_)
        {
            QString testName{"Numeric ranges test for " + fileNames_[i] + " " +
                             extension};
            QTest::newRow(testName.toStdString().c_str())
                << fileNames_[i] + "." + extension << expectedRanges[i];
        }
}

void DetailedSpreadsheetsTest::testNumericColumnRanges()
{
    QFETCH(QString, fileName);
    QFETCH(QVector<NumericCheckData>, expectedRanges);

    QString filePath(Common::getSpreadsheetsDir() + fileName);
    std::unique_ptr<DatasetSpreadsheet> dataset{
        Common::createDataset(filePath)};

    prepareDatasetForTest(dataset);

    for (const auto& [columnIndex, expectedMin, expectedMax] : expectedRanges)
        checkNumericColumnRange(dataset, columnIndex,
                                {expectedMin, expectedMax});
}

void DetailedSpreadsheetsTest::testDataFile01_data()
{
    QTest::addColumn<QString>("fileName");
    for (const auto& extension : QVector<QString>{"xlsx", "ods"})
    {
        QString testName{"Detailed test for test01 " + extension};
        QTest::newRow(testName.toStdString().c_str()) << "test01." + extension;
    }
}

void DetailedSpreadsheetsTest::testDataFile01()
{
    QFETCH(QString, fileName);

    QString filePath(Common::getSpreadsheetsDir() + fileName);
    std::unique_ptr<DatasetSpreadsheet> dataset{
        Common::createDataset(filePath)};

    prepareDatasetForTest(dataset);

    checkDateColumnRange(dataset, 2, {QDate(2010, 1, 1), QDate(2011, 8, 31)},
                         false);

    QStringList compareList = {"brown", "red",   "yellow",    "black", "blue",
                               "pink",  "white", "dark blue", "orange"};
    checkStringColumnRange(dataset, 6, compareList);
}

void DetailedSpreadsheetsTest::testDataFile01SomeColumns_data()
{
    QTest::addColumn<QString>("fileName");
    for (const auto& extension : QVector<QString>{"xlsx", "ods"})
    {
        QString testName{"Detailed test for test01Partial " + extension};
        QTest::newRow(testName.toStdString().c_str())
            << "test01Partial." + extension;
    }
}

void DetailedSpreadsheetsTest::testDataFile01SomeColumns()
{
    QFETCH(QString, fileName);
    QString filePath(Common::getSpreadsheetsDir() + fileName);
    std::unique_ptr<DatasetSpreadsheet> dataset{
        Common::createDataset(filePath)};

    dataset->initialize();
    QVector<bool> activeColumns(dataset->columnCount(), true);
    activeColumns[0] = false;
    activeColumns[1] = false;
    activeColumns[4] = false;
    dataset->setActiveColumns(activeColumns);
    dataset->loadData();

    checkNumericColumnRange(dataset, 1, {14.91, 126.69});
    checkNumericColumnRange(dataset, 2, {803.25, 39999.98});

    checkDateColumnRange(dataset, 0, {QDate(2010, 1, 1), QDate(2011, 8, 31)},
                         false);

    QStringList compareList = {"brown", "red",   "yellow",    "black", "blue",
                               "pink",  "white", "dark blue", "orange"};
    checkStringColumnRange(dataset, 3, compareList);

    Common::compareExportDataWithDump(std::move(dataset));
}

void DetailedSpreadsheetsTest::testDataFile03_data()
{
    QString file("test03");
    QTest::addColumn<QString>("fileName");
    for (const auto& extension : QVector<QString>{"xlsx", "ods"})
    {
        QString testName{"Detailed test for " + file + " " + extension};
        QTest::newRow(testName.toStdString().c_str()) << file + "." + extension;
    }
}

void DetailedSpreadsheetsTest::testDataFile03()
{
    QFETCH(QString, fileName);

    QString filePath(Common::getSpreadsheetsDir() + fileName);
    std::unique_ptr<DatasetSpreadsheet> dataset{
        Common::createDataset(filePath)};

    prepareDatasetForTest(dataset);

    checkDateColumnRange(dataset, 3, {QDate(2012, 2, 2), QDate(2012, 2, 5)},
                         true);

    QStringList compareList = {"a", "b", "c"};
    checkStringColumnRange(dataset, 4, compareList);
}

void DetailedSpreadsheetsTest::testDataFile04_data()
{
    QTest::addColumn<QString>("fileName");
    for (const auto& extension : QVector<QString>{"xlsx", "ods"})
    {
        QString testName{"Detailed test for test04 " + extension};
        QTest::newRow(testName.toStdString().c_str()) << "test04." + extension;
    }
}

void DetailedSpreadsheetsTest::testDataFile04()
{
    QFETCH(QString, fileName);

    QString filePath(Common::getSpreadsheetsDir() + fileName);
    std::unique_ptr<DatasetSpreadsheet> dataset{
        Common::createDataset(filePath)};

    prepareDatasetForTest(dataset);

    checkDateColumnRange(dataset, 1, {QDate(1970, 1, 1), QDate(1970, 1, 30)},
                         false);

    QStringList compareList;
    checkStringColumnRange(dataset, 0, compareList);
}

void DetailedSpreadsheetsTest::checkColumnFormats(
    const std::unique_ptr<DatasetSpreadsheet>& dataset,
    const QVector<ColumnType>& columnFormats)
{
    unsigned int column{0};
    for (auto& expectedColumnType : columnFormats)
        QCOMPARE(dataset->getColumnFormat(column++), expectedColumnType);
}

void DetailedSpreadsheetsTest::checkColumnNames(
    const std::unique_ptr<DatasetSpreadsheet>& dataset,
    const QVector<QString>& columnNames)
{
    unsigned int column{0};
    for (auto& expectedColumnName : columnNames)
        QCOMPARE(dataset->getHeaderName(column++), expectedColumnName);
}

void DetailedSpreadsheetsTest::checkNumericColumnRange(
    const std::unique_ptr<DatasetSpreadsheet>& dataset, int columnIndex,
    std::pair<double, double> expectedRange)
{
    auto [currentMin, currentMax] = dataset->getNumericRange(columnIndex);
    const auto [expectedMin, expectedMax] = expectedRange;
    QCOMPARE(currentMin, expectedMin);
    QCOMPARE(currentMax, expectedMax);
}

void DetailedSpreadsheetsTest::checkDateColumnRange(
    const std::unique_ptr<DatasetSpreadsheet>& dataset, int columnIndex,
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
    const std::unique_ptr<DatasetSpreadsheet>& dataset, int columnIndex,
    QStringList& expectedList)
{
    QStringList currentList = dataset->getStringList(columnIndex);
    QCOMPARE(currentList, expectedList);
}

void DetailedSpreadsheetsTest::prepareDatasetForTest(
    std::unique_ptr<DatasetSpreadsheet>& dataset)
{
    dataset->initialize();
    Common::activateAllDatasetColumns(dataset);
    dataset->loadData();
}
