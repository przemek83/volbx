#include "InnerTests.h"

#include <Qt5Quazip/quazipfile.h>
#include <QBuffer>
#include <QDirIterator>
#include <QDomDocument>
#include <QTableView>
#include <QtTest/QtTest>

#include <Common/FileUtilities.h>
#include <Datasets/DatasetInner.h>
#include <Export/ExportVbx.h>
#include <ModelsAndViews/FilteringProxyModel.h>
#include <ModelsAndViews/TableModel.h>

#include "Common.h"
#include "DatasetCommon.h"
#include "DatasetUtilities.h"

void InnerTests::initTestCase() { generateDumpData(); }

void InnerTests::testDefinition_data()
{
    addTestCases(QStringLiteral("Test definition"));
}

void InnerTests::testDefinition()
{
    QFETCH(QString, datasetName);
    DatasetCommon::checkDefinition(datasetName,
                                   DatasetUtilities::getDatasetsDir());
}

void InnerTests::testData_data() { addTestCases(QStringLiteral("Test data")); }

void InnerTests::testData()
{
    QFETCH(QString, datasetName);
    DatasetCommon::checkData(datasetName, DatasetUtilities::getDatasetsDir());
}

void InnerTests::testExport_data()
{
    addTestCases(QStringLiteral("Test export"));
}

void InnerTests::testExport()
{
    QFETCH(QString, datasetName);

    QByteArray exportedByteArray;
    QBuffer exportedBuffer(&exportedByteArray);
    exportedBuffer.open(QIODevice::WriteOnly);
    generateVbxFile(datasetName, exportedBuffer, {});

    checkExport(datasetName, exportedBuffer);
}

void InnerTests::generateDumpData()
{
    QString generatedFilesDir{QApplication::applicationDirPath() +
                              "/generatedVbxTestData/"};
    QDir().mkdir(generatedFilesDir);
    for (const auto& datasetName : testFileNames_)
        DatasetCommon::generateExpectedDataForFile(
            datasetName, DatasetUtilities::getDatasetsDir(), generatedFilesDir);
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
    for (int i = 0; i < actualDataLines.size(); ++i)
        if (actualDataLines[i] != expectedDataLines[i])
        {
            QString msg{"Difference in line " + QString::number(i + 1) +
                        "\nActual:  " + actualDataLines[i] +
                        "\nExpected: " + expectedDataLines[i]};
            QFAIL(msg.toStdString().c_str());
        }
}

void InnerTests::checkExportedDefinitions(QuaZip& zipOriginal,
                                          QuaZip& zipGenerated)
{
    const QByteArray originalData{loadDataFromZip(
        zipOriginal, DatasetUtilities::getDatasetDefinitionFilename())};
    const QByteArray generatedData{loadDataFromZip(
        zipGenerated, DatasetUtilities::getDatasetDefinitionFilename())};
    DatasetCommon::xmlsAreEqual(generatedData, originalData);
}

void InnerTests::generateVbxFile(const QString& datasetName, QBuffer& buffer,
                                 const QVector<bool>& activeColumns)
{
    std::unique_ptr<Dataset> dataset{DatasetCommon::createDataset(
        datasetName, DatasetUtilities::getDatasetsDir())};
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
    QuaZip zipOriginal(DatasetUtilities::getDatasetsDir() + datasetName +
                       DatasetUtilities::getDatasetExtension());
    QVERIFY(zipOriginal.open(QuaZip::mdUnzip));

    QuaZip zipGenerated(&exportedBuffer);
    QVERIFY(zipGenerated.open(QuaZip::mdUnzip));

    checkExportedData(DatasetUtilities::getDatasetDataFilename(), zipOriginal,
                      zipGenerated);

    checkExportedData(DatasetUtilities::getDatasetStringsFilename(),
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

void InnerTests::addTestCases(const QString& testNamePrefix)
{
    QTest::addColumn<QString>("datasetName");

    for (const auto& fileName : testFileNames_)
    {
        QString testName{testNamePrefix + " for " + fileName};
        QTest::newRow(testName.toStdString().c_str()) << fileName;
    }
}
