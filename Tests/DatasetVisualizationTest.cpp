#include "DatasetVisualizationTest.h"

#include <QtTest/QtTest>

#include <Dataset.h>
#include <DatasetVisualization.h>

#include "Common.h"
#include "DatasetCommon.h"

std::unique_ptr<Dataset> DatasetVisualizationTest::getDataset() const
{
    const QString fileName{"test.ods"};
    const QString filePath(Common::getSpreadsheetsDir() + fileName);
    return DatasetCommon::createDataset(fileName, filePath);
}

std::unique_ptr<Dataset> getDataset();
void DatasetVisualizationTest::testGetDataset() const
{
    std::unique_ptr<Dataset> expectedDataset{getDataset()};

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

void DatasetVisualizationTest::testClearDataset() const
{
    std::unique_ptr<Dataset> expectedDataset{getDataset()};
    DatasetVisualization visualization(nullptr);
    visualization.setDataset(std::move(expectedDataset));

    visualization.clear();

    auto dataset{visualization.retrieveDataset()};
    QCOMPARE(dataset, nullptr);
}
