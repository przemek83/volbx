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

    for (const auto& extension : extensions_)
        for (int i = 0; i < fileNames_.size(); ++i)
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

    QString filePath(getSpreadsheetsDir() + fileName);
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

    for (const auto& extension : extensions_)
        for (int i = 0; i < fileNames_.size(); ++i)
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

    QString filePath(getSpreadsheetsDir() + fileName);
    std::unique_ptr<DatasetSpreadsheet> dataset{
        Common::createDataset(filePath)};
    dataset->initialize();

    checkColumnFormats(dataset, columnFormats);
    checkColumnNames(dataset, columnNames);
    checkTaggedColumnsNotSet(dataset);
}

void DetailedSpreadsheetsTest::testSampleData_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<unsigned int>("sampleRowCount");
    QTest::addColumn<unsigned int>("sampleColumnCount");
    QTest::addColumn<QVector<Field>>("sampleFields");

    const QVector<unsigned int> expectedSampleRowCounts{10, 4, 10};
    const QVector<unsigned int> expectedSampleColumnCounts{7, 5, 12};

    for (const auto& extension : extensions_)
        for (int i = 0; i < fileNames_.size(); ++i)
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

    QString filePath(getSpreadsheetsDir() + fileName);
    std::unique_ptr<DatasetSpreadsheet> dataset{
        Common::createDataset(filePath)};
    dataset->initialize();

    QVector<QVector<QVariant>> sampleData{dataset->retrieveSampleData()};

    QCOMPARE(sampleData.size(), sampleRowCount);
    QCOMPARE(sampleData.front().size(), sampleColumnCount);

    std::tuple<QVariant, int, int> fieldsTuple;
    for (auto [value, row, column] : sampleFields)
        QCOMPARE(sampleData.at(row)[column], value);
}

QString DetailedSpreadsheetsTest::getSpreadsheetsDir()
{
    return QString(":/TestFiles/TestSpreadsheets/");
}

void DetailedSpreadsheetsTest::testSpreadsheetFile01(
    std::unique_ptr<DatasetSpreadsheet> dataset, QString file)
{
    dataset->initialize();

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
    dataset->initialize();
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
    QString file("test03");
    QTest::addColumn<QString>("fileName");
    for (const auto& extension : QVector<QString>{"xlsx", "ods"})
    {
        QString testName{"Detailed test for " + file + " " + extension};
        QTest::newRow(testName.toStdString().c_str()) << file + "." + extension;
    }
}

void DetailedSpreadsheetsTest::testDetailedSpreadsheetFile03()
{
    QFETCH(QString, fileName);

    QString filePath(getSpreadsheetsDir() + fileName);
    std::unique_ptr<DatasetSpreadsheet> dataset{
        Common::createDataset(filePath)};

    dataset->initialize();

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

void DetailedSpreadsheetsTest::testSpreadsheetFile04(
    std::unique_ptr<DatasetSpreadsheet> dataset, QString file)
{
    dataset->initialize();

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

void DetailedSpreadsheetsTest::checkTaggedColumnsNotSet(
    const std::unique_ptr<DatasetSpreadsheet>& dataset)
{
    unsigned int column{0};
    bool ok{false};
    std::tie(ok, column) = dataset->getTaggedColumn(ColumnTag::DATE);
    QVERIFY(!ok);
    std::tie(ok, column) = dataset->getTaggedColumn(ColumnTag::VALUE);
    QVERIFY(!ok);
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
