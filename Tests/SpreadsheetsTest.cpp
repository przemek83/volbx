#include "SpreadsheetsTest.h"

#include <memory>

#include <QApplication>
#include <QClipboard>
#include <QString>
#include <QTableView>
#include <QtTest/QtTest>

#include "Common/Configuration.h"
#include "Common/Constants.h"
#include "Common/FileUtilities.h"
#include "Datasets/Dataset.h"
#include "Datasets/DatasetOds.h"
#include "Datasets/DatasetXlsx.h"
#include "ModelsAndViews/FilteringProxyModel.h"
#include "ModelsAndViews/TableModel.h"

#include "Common.h"

void SpreadsheetsTest::initTestCase()
{
    // generateAllDumpData();
}

QString SpreadsheetsTest::getSpreadsheetsDir()
{
    return QString(QApplication::applicationDirPath() +
                   QLatin1String("/TestSpreadsheets/"));
}

void SpreadsheetsTest::testCompareSpreadsheetFiles_data()
{
    addTestCasesForFileNames(fileNames_);
}

void SpreadsheetsTest::testCompareSpreadsheetFiles()
{
    QFETCH(QString, fileName);

    std::unique_ptr<DatasetSpreadsheet> dataset{createDataset(fileName)};
    QVERIFY(dataset->initialize());

    compareDatasetDefinitionWithDump(dataset, fileName);

    activateAllDatasetColumns(dataset);
    QVERIFY(dataset->loadData());
    QVERIFY(dataset->isValid());

    Common::compareExportDataWithDump(std::move(dataset));
}

void SpreadsheetsTest::testCompareSpreadsheetFilesDamaged_data()
{
    addTestCasesForFileNames({"damaged"});
}

void SpreadsheetsTest::testCompareSpreadsheetFilesDamaged()
{
    QFETCH(QString, fileName);

    std::unique_ptr<DatasetSpreadsheet> dataset{createDataset(fileName)};
    QVERIFY(!dataset->initialize());
}

void SpreadsheetsTest::testCompareDefinitionDumps_data()
{
    addTestDataForDumpsComparison("Compare definition dumps");
}

void SpreadsheetsTest::testCompareDefinitionDumps()
{
    compareDumps(Common::getDefinitionDumpSuffix());
}

void SpreadsheetsTest::testCompareTsvDumps_data()
{
    addTestDataForDumpsComparison("Compare tsv dumps");
}

void SpreadsheetsTest::testCompareTsvDumps()
{
    compareDumps(Common::getDataTsvDumpSuffix());
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

void SpreadsheetsTest::addTestDataForDumpsComparison(
    const QString& testNamePrefix)
{
    QTest::addColumn<QString>("fileName");

    for (const auto& fileName : fileNames_)
    {
        QString testName{testNamePrefix + " for " + fileName};
        QTest::newRow(testName.toStdString().c_str()) << fileName;
    }
}

void SpreadsheetsTest::compareDumps(const QString& fileSuffix)
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

void SpreadsheetsTest::compareDatasetDefinitionWithDump(
    const std::unique_ptr<DatasetSpreadsheet>& dataset, const QString& fileName)
{
    QString filePath{getSpreadsheetsDir() + fileName};
    const QString dumpFileName{filePath + Common::getDefinitionDumpSuffix()};
    QByteArray dumpFromFile{
        FileUtilities::loadFile(dumpFileName).second.toUtf8()};
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

void SpreadsheetsTest::generateAllDumpData()
{
    QDirIterator dirIt(getSpreadsheetsDir(), QDirIterator::Subdirectories);
    while (dirIt.hasNext())
    {
        dirIt.next();
        QFileInfo fileInfo(dirIt.filePath());
        if (fileInfo.isFile() &&
            (fileInfo.suffix() == "xlsx" || fileInfo.suffix() == "ods"))
            generateDataDumpsForFile(fileInfo.filePath());
    }
}

void SpreadsheetsTest::generateDataDumpsForFile(QString name)
{
    std::unique_ptr<DatasetSpreadsheet> dataset{nullptr};
    if (name.endsWith(".xlsx"))
        dataset = std::make_unique<DatasetXlsx>(name, name);
    else
        dataset = std::make_unique<DatasetOds>(name, name);

    dataset->initialize();
    QByteArray dumpedDefinition = dataset->definitionToXml(dataset->rowCount());
    Common::saveFile(name + Common::getDefinitionDumpSuffix(),
                     dumpedDefinition);

    if (name.contains("damagedFiles"))
        return;

    QVector<bool> activeColumns(dataset->columnCount(), true);
    dataset->setActiveColumns(activeColumns);
    dataset->loadData();

    TableModel model(std::move(dataset));
    FilteringProxyModel proxyModel;
    proxyModel.setSourceModel(&model);

    QTableView view;
    view.setModel(&proxyModel);

    QString tsvData{Common::getExportedTsv(view)};
    Common::saveFile(name + Common::getDataTsvDumpSuffix(), tsvData);
}
