#include "DatasetCommon.h"

#include <memory>

#include <Common/FileUtilities.h>
#include <Datasets/Dataset.h>
#include <QtTest/QtTest>
#include "Common.h"

namespace
{
void checkDatasetDefinition(const std::unique_ptr<Dataset>& dataset,
                            const QString& expectedDefinitionFileName)
{
    QByteArray dumpFromFile{
        FileUtilities::loadFile(expectedDefinitionFileName).second.toUtf8()};
    QByteArray dumpFromDataset{dataset->definitionToXml(dataset->rowCount())};
    QVERIFY(Common::xmlsAreEqual(dumpFromFile, dumpFromDataset));
}
};  // namespace

namespace DatasetCommon
{
void checkDefinition(const QString& fileName)
{
    std::unique_ptr<Dataset> dataset{
        Common::createDataset(Common::getSpreadsheetsDir() + fileName)};
    QVERIFY(dataset->initialize());
    QVERIFY(dataset->isValid());

    const QString expectedDefinitionFileName{Common::getSpreadsheetsDir() +
                                             fileName +
                                             Common::getDefinitionDumpSuffix()};
    checkDatasetDefinition(dataset, expectedDefinitionFileName);
}
};  // namespace DatasetCommon
