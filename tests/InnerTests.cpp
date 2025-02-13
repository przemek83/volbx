#include "InnerTests.h"

#include <quazip/quazipfile.h>
#include <QBuffer>
#include <QDirIterator>
#include <QDomDocument>
#include <QTableView>
#include <QTest>

#include <Common/FileUtilities.h>
#include <Datasets/DatasetInner.h>
#include <Export/ExportVbx.h>
#include <ModelsAndViews/FilteringProxyModel.h>
#include <ModelsAndViews/TableModel.h>

#include "Common.h"
#include "DatasetCommon.h"
#include "DatasetUtilities.h"

void InnerTests::initTestCase()
{
    // generateDumpData();
}

void InnerTests::testDefinition_data()
{
    addTestCases(QStringLiteral("Test definition"));
}

void InnerTests::testDefinition()
{
    QFETCH(const QString, datasetName);
    DatasetCommon::checkDefinition(datasetName,
                                   dataset_utilities::getDatasetsDir());
}

void InnerTests::testData_data() { addTestCases(QStringLiteral("Test data")); }

void InnerTests::testData()
{
    QFETCH(const QString, datasetName);
    DatasetCommon::checkData(datasetName, dataset_utilities::getDatasetsDir());
}

void InnerTests::testExport_data()
{
    addTestCases(QStringLiteral("Test export"));
}

void InnerTests::testExport()
{
    QFETCH(const QString, datasetName);

    QByteArray exportedByteArray;
    QBuffer exportedBuffer(&exportedByteArray);
    exportedBuffer.open(QIODevice::WriteOnly);
    generateVbxFile(datasetName, exportedBuffer, {});

    checkExport(datasetName, exportedBuffer);
}

void InnerTests::generateDumpData()
{
    const QString generatedFilesDir{QApplication::applicationDirPath() +
                                    "/generatedVbxTestData/"};
    QDir().mkdir(generatedFilesDir);
    for (const auto& datasetName : testFileNames_)
        DatasetCommon::generateExpectedDataForFile(
            datasetName, dataset_utilities::getDatasetsDir(),
            generatedFilesDir);
}

QByteArray InnerTests::loadDataFromZip(QuaZip& zip, const QString& fileName)
{
    QuaZipFile zipFileOriginal(&zip);
    zip.setCurrentFile(fileName);
    zipFileOriginal.open(QIODevice::ReadOnly);
    return zipFileOriginal.readAll();
}

void InnerTests::checkExportedData(const QString& fileName, QuaZip& zipOriginal,
                                   QuaZip& zipGenerated)
{
    const QByteArray originalData{loadDataFromZip(zipOriginal, fileName)};
    const QByteArray generatedData{loadDataFromZip(zipGenerated, fileName)};
    QList<QByteArray> actualDataLines{generatedData.split('\n')};
    QList<QByteArray> expectedDataLines{originalData.split('\n')};
    QCOMPARE(actualDataLines.size(), expectedDataLines.size());

    qsizetype linesCount{actualDataLines.size()};
    for (qsizetype i{0}; i < linesCount; ++i)
    {
        const QByteArray& actualLine{actualDataLines[i]};
        const QByteArray& expectedLine{expectedDataLines[i]};
        if (actualLine != expectedLine)
        {
            const QString msg{"Difference in line " + QString::number(i + 1) +
                              "\nActual:  " + actualLine +
                              "\nExpected: " + expectedLine};
            QFAIL(msg.toStdString().c_str());
        }
    }
}

void InnerTests::checkExportedDefinitions(QuaZip& zipOriginal,
                                          QuaZip& zipGenerated)
{
    const QByteArray originalData{loadDataFromZip(
        zipOriginal, dataset_utilities::getDatasetDefinitionFilename())};
    const QByteArray generatedData{loadDataFromZip(
        zipGenerated, dataset_utilities::getDatasetDefinitionFilename())};
    DatasetCommon::xmlsAreEqual(generatedData, originalData);
}

void InnerTests::generateVbxFile(const QString& datasetName, QBuffer& buffer,
                                 const QVector<bool>& activeColumns)
{
    std::unique_ptr<Dataset> dataset{DatasetCommon::createDataset(
        datasetName, dataset_utilities::getDatasetsDir())};
    dataset->initialize();
    if (activeColumns.empty())
        DatasetCommon::activateAllDatasetColumns(*dataset);
    else
        dataset->setActiveColumns(activeColumns);
    dataset->loadData();

    TableModel model(std::move(dataset));
    FilteringProxyModel proxyModel;
    proxyModel.setSourceModel(&model);

    QTableView view;
    view.setModel(&proxyModel);

    ExportVbx exportVbx;
    exportVbx.generateVbx(view, buffer);
}

void InnerTests::checkExport(const QString& datasetName,
                             QBuffer& exportedBuffer)
{
    QuaZip zipOriginal(dataset_utilities::getDatasetsDir() + datasetName +
                       dataset_utilities::getDatasetExtension());
    QVERIFY(zipOriginal.open(QuaZip::mdUnzip));

    QuaZip zipGenerated(&exportedBuffer);
    QVERIFY(zipGenerated.open(QuaZip::mdUnzip));

    checkExportedData(dataset_utilities::getDatasetDataFilename(), zipOriginal,
                      zipGenerated);

    checkExportedData(dataset_utilities::getDatasetStringsFilename(),
                      zipOriginal, zipGenerated);

    checkExportedDefinitions(zipOriginal, zipGenerated);
}

void InnerTests::testPartialData()
{
    QByteArray exportedByteArray;
    QBuffer exportedBuffer(&exportedByteArray);
    exportedBuffer.open(QIODevice::WriteOnly);
    QVector<bool> activeColumns(7, false);
    activeColumns[1] = true;
    activeColumns[2] = true;
    activeColumns[5] = true;
    activeColumns[6] = true;
    generateVbxFile(QStringLiteral("ExampleData"), exportedBuffer,
                    activeColumns);

    checkExport(QStringLiteral("ExampleDataPartial"), exportedBuffer);
}

void InnerTests::addTestCases(const QString& testNamePrefix) const
{
    QTest::addColumn<QString>("datasetName");

    for (const auto& fileName : testFileNames_)
    {
        const QString testName{testNamePrefix + " for " + fileName};
        QTest::newRow(testName.toStdString().c_str()) << fileName;
    }
}
