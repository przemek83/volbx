#include "SpreadsheetsTest.h"

#include <QApplication>
#include <QTableView>
#include <QTest>

#include <Common/FileUtilities.h>
#include <Datasets/DatasetOds.h>
#include <Datasets/DatasetXlsx.h>
#include <ModelsAndViews/FilteringProxyModel.h>
#include <ModelsAndViews/TableModel.h>

#include "Common.h"
#include "DatasetCommon.h"

void SpreadsheetsTest::initTestCase() const
{
    // generateExpectedData();
}

void SpreadsheetsTest::testDefinition_data() const
{
    addTestCasesForFileNames(testFileNames_);
}

void SpreadsheetsTest::testDefinition()
{
    QFETCH(const QString, fileName);
    DatasetCommon::checkDefinition(fileName, Common::getSpreadsheetsDir());
}

void SpreadsheetsTest::testData_data() const
{
    addTestCasesForFileNames(testFileNames_);
}

void SpreadsheetsTest::testData()
{
    QFETCH(const QString, fileName);
    DatasetCommon::checkData(fileName, Common::getSpreadsheetsDir());
}

void SpreadsheetsTest::testDamagedFiles_data()
{
    addTestCasesForFileNames({"damaged"});
}

void SpreadsheetsTest::testDamagedFiles()
{
    QFETCH(const QString, fileName);

    std::unique_ptr<Dataset> dataset{DatasetCommon::createDataset(
        fileName, Common::getSpreadsheetsDir() + fileName)};
    QVERIFY(!dataset->initialize());
}

void SpreadsheetsTest::compareExpectedDefinitionsOfOdsAndXlsx_data() const
{
    addTestCaseForOdsAndXlsxComparison(
        QStringLiteral("Compare definition dumps"));
}

void SpreadsheetsTest::compareExpectedDefinitionsOfOdsAndXlsx()
{
    compareOdsAndXlsxExpectedData(Common::getDefinitionDumpSuffix());
}

void SpreadsheetsTest::compareExpectedTsvDumpsOfOdsAndXlsx_data() const
{
    addTestCaseForOdsAndXlsxComparison(QStringLiteral("Compare tsv dumps"));
}

void SpreadsheetsTest::compareExpectedTsvDumpsOfOdsAndXlsx()
{
    compareOdsAndXlsxExpectedData(Common::getDataTsvDumpSuffix());
}

void SpreadsheetsTest::addTestCaseForOdsAndXlsxComparison(
    const QString& testNamePrefix) const
{
    QTest::addColumn<QString>("fileName");

    for (const auto& fileName : testFileNames_)
    {
        const QString testName{testNamePrefix + " for " + fileName};
        QTest::newRow(testName.toStdString().c_str()) << fileName;
    }
}

void SpreadsheetsTest::compareOdsAndXlsxExpectedData(const QString& fileSuffix)
{
    QFETCH(const QString, fileName);

    const QString filePath{Common::getSpreadsheetsDir() + fileName};
    auto [xlsxLoaded,
          xlsxDump]{file_utilities::loadFile(filePath + ".xlsx" + fileSuffix)};
    QVERIFY(xlsxLoaded);

    auto [odsLoaded,
          odsDump]{file_utilities::loadFile(filePath + ".ods" + fileSuffix)};
    QVERIFY(odsLoaded);

    QStringList xlsxLines{xlsxDump.split('\n')};
    QStringList odsLines{odsDump.split('\n')};
    QCOMPARE(xlsxLines.size(), odsLines.size());

    const qsizetype linesCount{xlsxLines.size()};
    for (int i{0}; i < linesCount; ++i)
    {
        QString xlsxLine{xlsxLines[i]};
        QString odsLine{odsLines[i]};
        if (xlsxLine != odsLine)
        {
            const QString msg{"Difference in line " + QString::number(i + 1) +
                              "\nXlsx: " + xlsxLine + "\nOds : " + odsLine};
            QFAIL(msg.toStdString().c_str());
        }
    }
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

void SpreadsheetsTest::generateExpectedData() const
{
    const QString generatedFilesDir{QApplication::applicationDirPath() +
                                    "/generatedSpreadsheetsTestData/"};
    QDir().mkdir(generatedFilesDir);
    for (const auto& fileName : testFileNames_)
    {
        DatasetCommon::generateExpectedDataForFile(fileName + ".xlsx",
                                                   Common::getSpreadsheetsDir(),
                                                   generatedFilesDir);
        DatasetCommon::generateExpectedDataForFile(
            fileName + ".ods", Common::getSpreadsheetsDir(), generatedFilesDir);
    }
}
