#include "DatasetVisualizationTest.h"

#include <memory>

#include <QApplication>
#include <QTest>
#include <QTreeWidget>

#include <Dataset.h>
#include <DatasetVisualization.h>

#include "ColumnTag.h"
#include "Common.h"
#include "DatasetCommon.h"

std::unique_ptr<Dataset> DatasetVisualizationTest::getDataset()
{
    const QString fileName{QStringLiteral("smallDataSet.ods")};
    const QString filePath(Common::getSpreadsheetsDir() + fileName);
    std::unique_ptr<Dataset> dataset{
        DatasetCommon::createDataset(fileName, filePath)};
    dataset->initialize();
    dataset->setTaggedColumn(ColumnTag::VALUE, 0);
    dataset->setTaggedColumn(ColumnTag::DATE, 3);
    return dataset;
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

void DatasetVisualizationTest::testGetDatasetWithoutSettingIt()
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

void DatasetVisualizationTest::testSelectCurrentColumn() const
{
    std::unique_ptr<Dataset> dataset{getDataset()};
    DatasetVisualization visualization(nullptr);
    visualization.setDataset(std::move(dataset));

    const auto* tree{visualization.findChild<QTreeWidget*>()};
    auto* initialItem{tree->currentItem()};
    QCOMPARE(initialItem, nullptr);

    visualization.selectCurrentColumn(1);
    QCOMPARE(tree->currentItem()->text(0), "pow");
}
