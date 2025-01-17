#include "DatasetVisualizationTest.h"

#include <QtTest/QtTest>

#include <Dataset.h>
#include <DatasetVisualization.h>
#include <qtestcase.h>

#include "Common.h"
#include "DatasetCommon.h"

void DatasetVisualizationTest::testGetDataset() const
{
    const QString fileName{"test.ods"};
    const QString filePath(Common::getSpreadsheetsDir() + fileName);
    std::unique_ptr<Dataset> expectedDataset{
        DatasetCommon::createDataset(fileName, filePath)};

    QByteArray expectedDefinition{
        expectedDataset->definitionToXml(expectedDataset->rowCount())};

    DatasetVisualization visualization(nullptr);
    visualization.setDataset(std::move(expectedDataset));

    auto actualDataset{visualization.retrieveDataset()};
    QByteArray actualDefinition{
        actualDataset->definitionToXml(actualDataset->rowCount())};

    QCOMPARE(DatasetCommon::xmlsAreEqual(actualDefinition, expectedDefinition),
             true);
}

void DatasetVisualizationTest::testGetDatasetWithoutSettingIt() const
{
    DatasetVisualization visualization(nullptr);

    auto dataset{visualization.retrieveDataset()};

    QCOMPARE(dataset, nullptr);
}
