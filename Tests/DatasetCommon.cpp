#include "DatasetCommon.h"

#include <memory>

#include <Common/FileUtilities.h>
#include <Datasets/Dataset.h>
#include <QtTest/QtTest>
#include "Common.h"

namespace
{
void checkDatasetDefinition(const std::unique_ptr<Dataset>& dataset,
                            const QString& expectedDefinitionFilePath)
{
    QByteArray dumpFromFile{
        FileUtilities::loadFile(expectedDefinitionFilePath).second.toUtf8()};
    QByteArray dumpFromDataset{dataset->definitionToXml(dataset->rowCount())};
    QVERIFY(Common::xmlsAreEqual(dumpFromFile, dumpFromDataset));
}
};  // namespace

namespace DatasetCommon
{
void checkDefinition(const QString& fileName, const QString& dir)
{
    const QString filePath{dir + fileName};
    std::unique_ptr<Dataset> dataset{Common::createDataset(fileName, filePath)};
    QVERIFY(dataset->initialize());
    QVERIFY(dataset->isValid());

    checkDatasetDefinition(dataset,
                           filePath + Common::getDefinitionDumpSuffix());
}
};  // namespace DatasetCommon
