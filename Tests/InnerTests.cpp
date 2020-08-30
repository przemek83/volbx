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

void InnerTests::checkExportedData(const QString& fileName, QuaZip& zipOriginal,
                                   QuaZip& zipGenerated)
{
    QuaZipFile zipFileOriginal(&zipOriginal);
    zipOriginal.setCurrentFile(fileName);
    QVERIFY(zipFileOriginal.open(QIODevice::ReadOnly));
    QByteArray originalData = zipFileOriginal.readAll();

    QuaZipFile zipFileGenerated(&zipGenerated);
    zipGenerated.setCurrentFile(fileName);
    QVERIFY(zipFileGenerated.open(QIODevice::ReadOnly));
    QByteArray generatedData = zipFileGenerated.readAll();

    QCOMPARE(generatedData, originalData);
}

void InnerTests::checkExportedDefinitions(QuaZip& zipOriginal,
                                          QuaZip& zipGenerated)
{
    QuaZipFile zipFileOriginal(&zipOriginal);
    zipOriginal.setCurrentFile(
        DatasetUtilities::getDatasetDefinitionFilename());
    QVERIFY(zipFileOriginal.open(QIODevice::ReadOnly));
    QByteArray originalData = zipFileOriginal.readAll();
    zipFileOriginal.close();

    QuaZipFile zipFileGenerated(&zipGenerated);
    zipGenerated.setCurrentFile(
        DatasetUtilities::getDatasetDefinitionFilename());
    QVERIFY(zipFileGenerated.open(QIODevice::ReadOnly));
    QByteArray generatedData = zipFileGenerated.readAll();
    zipFileGenerated.close();
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
    //    DatasetDefinitionInner* definition =
    //        new DatasetDefinitionInner("test");

    //    QVERIFY(true == definition->isValid());

    //    QVector<bool> activeColumns(definition->columnCount(), false);
    //    activeColumns[0] = true;
    //    activeColumns[1] = true;
    //    activeColumns[4] = true;
    //    activeColumns[5] = true;
    //    activeColumns[8] = true;
    //    activeColumns[13] = true;
    //    definition->setActiveColumns(activeColumns);

    //    DatasetInner* dataset = new DatasetInner(definition);
    //    dataset->init();

    //    QVERIFY(true == dataset->isValid());

    //    TableModel model(dataset);
    //    FilteringProxyModel proxyModel;
    //    proxyModel.setSourceModel(&model);

    //    QTableView view;
    //    view.setModel(&proxyModel);
    //    view.setSelectionMode(QAbstractItemView::MultiSelection);
    //    view.setSelectionBehavior(QAbstractItemView::SelectRows);
    //    view.selectAll();
    //    QItemSelectionModel* selectionModel = view.selectionModel();
    //    selectionModel->setCurrentIndex(model.index(1, 0),
    //                                    QItemSelectionModel::Deselect);
    //    ExportData::saveDataset(tempFilename_, &view);

    //    checkExport("partialTest");
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
