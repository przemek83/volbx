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

const QVector<QString> InnerTests::testFileNames_{
    "ExampleData", "po0_dmg", "po0_dmg2_bez_dat", "po0", "po1", "pustePola"};

void InnerTests::initTestCase()
{
    // generateDumpData();
}

void InnerTests::testDefinition_data() { addTestCases("Test definition"); }

void InnerTests::testDefinition()
{
    QFETCH(QString, datasetName);
    DatasetCommon::checkDefinition(datasetName,
                                   DatasetUtilities::getDatasetsDir());
}

void InnerTests::testData_data() { addTestCases("Test data"); }

void InnerTests::testData()
{
    QFETCH(QString, datasetName);
    DatasetCommon::checkData(datasetName, DatasetUtilities::getDatasetsDir());
}

void InnerTests::testExport_data() { addTestCases("Test export"); }

void InnerTests::testExport()
{
    QFETCH(QString, datasetName);

    std::unique_ptr<Dataset> dataset{DatasetCommon::createDataset(
        datasetName, DatasetUtilities::getDatasetsDir())};
    dataset->initialize();
    DatasetCommon::activateAllDatasetColumns(*dataset);
    dataset->loadData();

    TableModel model(std::move(dataset));
    FilteringProxyModel proxyModel;
    proxyModel.setSourceModel(&model);

    QTableView view;
    view.setModel(&proxyModel);

    QByteArray exportedByteArray;
    QBuffer exportedBuffer(&exportedByteArray);
    exportedBuffer.open(QIODevice::WriteOnly);
    ExportVbx exportVbx;
    exportVbx.generateVbx(view, exportedBuffer);

    checkExport(datasetName, exportedBuffer);
}

void InnerTests::generateDumpData()
{
    QDirIterator dirIt(DatasetUtilities::getDatasetsDir(),
                       QDirIterator::Subdirectories);
    while (dirIt.hasNext())
    {
        dirIt.next();
        QFileInfo fileInfo(dirIt.filePath());
        if (fileInfo.isFile() && (fileInfo.suffix() == "vbx"))
        {
            DatasetInner* dataset = new DatasetInner(fileInfo.baseName());
            dataset->initialize();
            QByteArray definitionDump =
                dataset->definitionToXml(dataset->rowCount());
            Common::saveFile(DatasetUtilities::getDatasetsDir() +
                                 fileInfo.baseName() +
                                 Common::getDefinitionDumpSuffix(),
                             definitionDump);

            QVector<bool> activeColumns(dataset->columnCount(), true);
            dataset->setActiveColumns(activeColumns);
            dataset->loadData();

            TableModel model((std::unique_ptr<Dataset>(dataset)));
            FilteringProxyModel proxyModel;
            proxyModel.setSourceModel(&model);

            QTableView view;
            view.setModel(&proxyModel);

            QString tsvData{DatasetCommon::getExportedTsv(view)};
            Common::saveFile(DatasetUtilities::getDatasetsDir() +
                                 fileInfo.baseName() +
                                 Common::getDataTsvDumpSuffix(),
                             tsvData);
        }
    }
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
    QCOMPARE(generatedData, originalData);
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
    std::unique_ptr<Dataset> dataset{DatasetCommon::createDataset(
        "ExampleData", DatasetUtilities::getDatasetsDir())};

    QVERIFY(dataset->initialize());
    QVERIFY(dataset->isValid());

    QVector<bool> activeColumns(dataset->columnCount(), false);
    activeColumns[1] = true;
    activeColumns[2] = true;
    activeColumns[5] = true;
    activeColumns[6] = true;
    dataset->setActiveColumns(activeColumns);
    dataset->loadData();

    TableModel model(std::move(dataset));
    FilteringProxyModel proxyModel;
    proxyModel.setSourceModel(&model);

    QTableView view;
    view.setModel(&proxyModel);

    QByteArray exportedByteArray;
    QBuffer exportedBuffer(&exportedByteArray);
    exportedBuffer.open(QIODevice::WriteOnly);
    ExportVbx exportVbx;
    exportVbx.generateVbx(view, exportedBuffer);

    checkExport("ExampleDataPartial", exportedBuffer);
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
