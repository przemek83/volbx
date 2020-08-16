#include "SpreadsheetsTest.h"

#include <QApplication>
#include <QTableView>
#include <QtTest/QtTest>

#include <Common/FileUtilities.h>
#include <Datasets/DatasetOds.h>
#include <Datasets/DatasetXlsx.h>
#include <ModelsAndViews/FilteringProxyModel.h>
#include <ModelsAndViews/TableModel.h>

#include "Common.h"

void SpreadsheetsTest::initTestCase()
{
    // generateDumpData();
}

QString SpreadsheetsTest::getSpreadsheetsDir()
{
    return QString(":/TestFiles/TestSpreadsheets/");
}

void SpreadsheetsTest::testFiles_data()
{
    addTestCasesForFileNames(testFileNames_);
}

void SpreadsheetsTest::testFiles()
{
    QFETCH(QString, fileName);

    std::unique_ptr<DatasetSpreadsheet> dataset{createDataset(fileName)};
    QVERIFY(dataset->initialize());

    const QString expectedDefinitionFileName{getSpreadsheetsDir() + fileName +
                                             Common::getDefinitionDumpSuffix()};
    checkDatasetDefinition(dataset, expectedDefinitionFileName);

    activateAllDatasetColumns(dataset);
    QVERIFY(dataset->loadData());
    QVERIFY(dataset->isValid());

    Common::compareExportDataWithDump(std::move(dataset));
}

void SpreadsheetsTest::testDamagedFiles_data()
{
    addTestCasesForFileNames({"damaged"});
}

void SpreadsheetsTest::testDamagedFiles()
{
    QFETCH(QString, fileName);

    std::unique_ptr<DatasetSpreadsheet> dataset{createDataset(fileName)};
    QVERIFY(!dataset->initialize());
}

void SpreadsheetsTest::compareExpectedDefinitionsOfOdsAndXlsx_data()
{
    addTestCaseForOdsAndXlsxComparison("Compare definition dumps");
}

void SpreadsheetsTest::compareExpectedDefinitionsOfOdsAndXlsx()
{
    compareOdsAndXlsxExpectedData(Common::getDefinitionDumpSuffix());
}

void SpreadsheetsTest::compareExpectedTsvDumpsOfOdsAndXlsx_data()
{
    addTestCaseForOdsAndXlsxComparison("Compare tsv dumps");
}

void SpreadsheetsTest::compareExpectedTsvDumpsOfOdsAndXlsx()
{
    compareOdsAndXlsxExpectedData(Common::getDataTsvDumpSuffix());
}

std::unique_ptr<DatasetSpreadsheet> SpreadsheetsTest::createDataset(
    const QString& fileName)
{
    QString filePath{getSpreadsheetsDir() + fileName};
    std::unique_ptr<DatasetSpreadsheet> dataset{nullptr};
    if (fileName.endsWith(".xlsx"))
        dataset = std::make_unique<DatasetXlsx>(filePath, filePath);
    else
        dataset = std::make_unique<DatasetOds>(filePath, filePath);
    return dataset;
}

void SpreadsheetsTest::addTestCaseForOdsAndXlsxComparison(
    const QString& testNamePrefix)
{
    QTest::addColumn<QString>("fileName");

    for (const auto& fileName : testFileNames_)
    {
        QString testName{testNamePrefix + " for " + fileName};
        QTest::newRow(testName.toStdString().c_str()) << fileName;
    }
}

void SpreadsheetsTest::compareOdsAndXlsxExpectedData(const QString& fileSuffix)
{
    QFETCH(QString, fileName);

    QString filePath{getSpreadsheetsDir() + fileName};
    auto [xlsxLoaded, xlsxDump] =
        FileUtilities::loadFile(filePath + ".xlsx" + fileSuffix);
    QVERIFY(xlsxLoaded);

    auto [odsLoaded, odsDump] =
        FileUtilities::loadFile(filePath + ".ods" + fileSuffix);
    QVERIFY(odsLoaded);

    QCOMPARE(xlsxDump, odsDump);
}

void SpreadsheetsTest::checkDatasetDefinition(
    const std::unique_ptr<DatasetSpreadsheet>& dataset,
    const QString& expectedDefinitionFileName)
{
    QByteArray dumpFromFile{
        FileUtilities::loadFile(expectedDefinitionFileName).second.toUtf8()};
    QByteArray dumpFromDataset{dataset->definitionToXml(dataset->rowCount())};
    QVERIFY(Common::xmlsAreEqual(dumpFromFile, dumpFromDataset));
}

void SpreadsheetsTest::activateAllDatasetColumns(
    const std::unique_ptr<DatasetSpreadsheet>& dataset)
{
    QVector<bool> activeColumns(dataset->columnCount(), true);
    dataset->setActiveColumns(activeColumns);
}

void SpreadsheetsTest::addTestCasesForFileNames(
    const QVector<QString>& fileNames)
{
    QTest::addColumn<QString>("fileName");

    for (const auto& fileName : fileNames)
    {
        QString testFileName{fileName + ".xlsx"};
        QString testName{"Test " + testFileName};
        QTest::newRow(testName.toStdString().c_str()) << testFileName;
        testFileName = fileName + ".ods";
        testName = "Test " + testFileName;
        QTest::newRow(testName.toStdString().c_str()) << testFileName;
    }
}

void SpreadsheetsTest::generateExpectedData()
{
    for (const auto& fileName : testFileNames_)
    {
        generateExpectedDataForFile(fileName + ".xlsx");
        generateExpectedDataForFile(fileName + ".ods");
    }
}

void SpreadsheetsTest::saveExpectedDefinition(
    const std::unique_ptr<DatasetSpreadsheet>& dataset, const QString& filePath)
{
    QByteArray dumpedDefinition{dataset->definitionToXml(dataset->rowCount())};
    Common::saveFile(filePath + Common::getDefinitionDumpSuffix(),
                     dumpedDefinition);
}

void SpreadsheetsTest::saveExpectedTsv(const QTableView& view,
                                       const QString& filePath)
{
    QString tsvData{Common::getExportedTsv(view)};
    Common::saveFile(filePath + Common::getDataTsvDumpSuffix(), tsvData);
}

void SpreadsheetsTest::generateExpectedDataForFile(const QString& fileName)
{
    std::unique_ptr<DatasetSpreadsheet> dataset{createDataset(fileName)};
    dataset->initialize();

    QString filePath{getSpreadsheetsDir() + fileName};
    saveExpectedDefinition(dataset, filePath);

    activateAllDatasetColumns(dataset);
    dataset->loadData();

    TableModel model(std::move(dataset));
    FilteringProxyModel proxyModel;
    proxyModel.setSourceModel(&model);
    QTableView view;
    view.setModel(&proxyModel);

    saveExpectedTsv(view, filePath);
}
