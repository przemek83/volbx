#include "DatasetCommon.h"

#include <memory>

#include <ExportDsv.h>
#include <QDomDocument>
#include <QTableView>
#include <QtTest/QtTest>

#include <Common/FileUtilities.h>
#include <Datasets/Dataset.h>
#include <Datasets/DatasetInner.h>
#include <Datasets/DatasetOds.h>
#include <Datasets/DatasetXlsx.h>
#include <ModelsAndViews/FilteringProxyModel.h>
#include <ModelsAndViews/TableModel.h>

#include "Common.h"

namespace
{
QHash<QString, QString> getAttributesMap(const QDomElement& element)
{
    QHash<QString, QString> attributeMap;
    const QDomNamedNodeMap attributes{element.attributes()};
    for (int i = 0; i < attributes.size(); ++i)
        attributeMap[attributes.item(i).toAttr().name()] =
            attributes.item(i).toAttr().value();
    return attributeMap;
}

bool atrributesEqual(QDomElement& left, QDomElement& right)
{
    return getAttributesMap(left) == getAttributesMap(right);
}

bool domElementsEqual(QDomElement& left, QDomElement& right);

bool domNodeListsEqual(const QDomNodeList& leftNodes,
                       const QDomNodeList& rightNodes)
{
    bool equal{true};
    for (int i = 0; i < leftNodes.size(); ++i)
    {
        bool found{false};
        for (int j = 0; j < rightNodes.size(); ++j)
        {
            QDomElement currentLeft{leftNodes.at(i).toElement()};
            QDomElement currentRight{rightNodes.at(j).toElement()};
            if (!currentLeft.hasChildNodes() && !currentRight.hasChildNodes())
            {
                if (atrributesEqual(currentLeft, currentRight))
                {
                    found = true;
                    break;
                }
                continue;
            }

            if (currentLeft.tagName() == currentRight.tagName())
            {
                equal = equal && domElementsEqual(currentLeft, currentRight);
                found = true;
            }
        }
        if (!found || !equal)
            return false;
    }
    return equal;
}

bool domElementsEqual(QDomElement& left, QDomElement& right)
{
    if (left.childNodes().count() != right.childNodes().count())
        return false;

    if (!left.hasChildNodes())
        return atrributesEqual(left, right);

    QDomNodeList leftNodes{left.childNodes()};
    QDomNodeList rightNodes{right.childNodes()};
    return domNodeListsEqual(leftNodes, rightNodes) &&
           domNodeListsEqual(rightNodes, leftNodes);
}

void saveExpectedDefinition(const std::unique_ptr<Dataset>& dataset,
                            const QString& filePath)
{
    QByteArray dumpedDefinition{dataset->definitionToXml(dataset->rowCount())};
    Common::saveFile(filePath + Common::getDefinitionDumpSuffix(),
                     dumpedDefinition);
}

void saveExpectedTsv(const QTableView& view, const QString& filePath)
{
    QString tsvData{DatasetCommon::getExportedTsv(view)};
    Common::saveFile(filePath + Common::getDataTsvDumpSuffix(), tsvData);
}
};  // namespace

namespace DatasetCommon
{
bool xmlsAreEqual(const QByteArray& left, const QByteArray& right)
{
    QDomDocument leftDom;
    leftDom.setContent(left);
    QDomDocument rightDom;
    rightDom.setContent(right);
    QDomElement leftRoot{leftDom.documentElement()};
    QDomElement rightRoot{rightDom.documentElement()};

    return domElementsEqual(leftRoot, rightRoot);
}

void checkDatasetDefinition(const std::unique_ptr<Dataset>& dataset,
                            const QString& expectedDefinitionFilePath)
{
    QByteArray dumpFromFile{
        FileUtilities::loadFile(expectedDefinitionFilePath).second.toUtf8()};
    QByteArray dumpFromDataset{dataset->definitionToXml(dataset->rowCount())};
    QVERIFY(xmlsAreEqual(dumpFromFile, dumpFromDataset));
}

QString getExportedTsv(const QAbstractItemView& view)
{
    QByteArray exportedByteArray;
    QBuffer exportedBuffer(&exportedByteArray);
    exportedBuffer.open(QIODevice::WriteOnly);

    ExportDsv exportDsv('\t');
    exportDsv.exportView(view, exportedBuffer);

    return QString::fromUtf8(exportedByteArray);
}

void compareExportDataWithDump(std::unique_ptr<Dataset> dataset,
                               const QString& filePath)
{
    QString datasetName{dataset->getName()};
    TableModel model(std::move(dataset));
    FilteringProxyModel proxyModel;
    proxyModel.setSourceModel(&model);

    QTableView view;
    view.setModel(&proxyModel);

    QString actualData{getExportedTsv(view)};
    QString expectedData{
        FileUtilities::loadFile(filePath + Common::getDataTsvDumpSuffix())
            .second};

    QVector<QStringRef> actualDataLines{actualData.splitRef('\n')};
    QVector<QStringRef> expectedDataLines{expectedData.splitRef('\n')};
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

std::unique_ptr<Dataset> createDataset(const QString& fileName,
                                       const QString& filePath)
{
    std::unique_ptr<Dataset> dataset{nullptr};
    if (fileName.endsWith(".xlsx"))
        dataset = std::make_unique<DatasetXlsx>(fileName, filePath);
    else if (fileName.endsWith(".ods"))
        dataset = std::make_unique<DatasetOds>(fileName, filePath);
    else
        dataset = std::make_unique<DatasetInner>(fileName);
    return dataset;
}

void activateAllDatasetColumns(Dataset& dataset)
{
    QVector<bool> activeColumns(dataset.columnCount(), true);
    dataset.setActiveColumns(activeColumns);
}

void checkDefinition(const QString& fileName, const QString& dir)
{
    const QString filePath{dir + fileName};
    std::unique_ptr<Dataset> dataset{createDataset(fileName, filePath)};
    QVERIFY(dataset->initialize());
    QVERIFY(dataset->isValid());

    checkDatasetDefinition(dataset,
                           filePath + Common::getDefinitionDumpSuffix());
}

void checkData(const QString& fileName, const QString& dir)
{
    const QString filePath(dir + fileName);
    std::unique_ptr<Dataset> dataset{createDataset(fileName, filePath)};
    dataset->initialize();

    activateAllDatasetColumns(*dataset);
    QVERIFY(dataset->loadData());
    QVERIFY(dataset->isValid());

    compareExportDataWithDump(std::move(dataset), filePath);
}

void generateExpectedDataForFile(const QString& fileName,
                                 const QString& sourceDir,
                                 const QString& destinationDir)
{
    std::unique_ptr<Dataset> dataset{
        DatasetCommon::createDataset(fileName, sourceDir + fileName)};
    dataset->initialize();

    QString filePath{destinationDir + fileName};
    saveExpectedDefinition(dataset, filePath);

    DatasetCommon::activateAllDatasetColumns(*dataset);
    dataset->loadData();

    TableModel model(std::move(dataset));
    FilteringProxyModel proxyModel;
    proxyModel.setSourceModel(&model);
    QTableView view;
    view.setModel(&proxyModel);

    saveExpectedTsv(view, filePath);
}
};  // namespace DatasetCommon
