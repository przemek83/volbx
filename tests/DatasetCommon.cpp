#include "DatasetCommon.h"

#include <memory>

#include <eible/ExportDsv.h>
#include <QBuffer>
#include <QDomDocument>
#include <QTableView>
#include <QTest>

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
    for (int i{0}; i < attributes.size(); ++i)
        attributeMap[attributes.item(i).toAttr().name()] =
            attributes.item(i).toAttr().value();
    return attributeMap;
}

bool atrributesEqual(const QDomElement& left, const QDomElement& right)
{
    return getAttributesMap(left) == getAttributesMap(right);
}

bool domElementsEqual(const QDomElement& left, const QDomElement& right);

bool haveSameElement(const QDomNodeList& rightNodes,
                     const QDomElement& leftElement)
{
    bool found{false};
    bool equal{true};
    for (int j{0}; j < rightNodes.size(); ++j)
    {
        const QDomElement rightElement{rightNodes.at(j).toElement()};
        if (leftElement.hasChildNodes() || rightElement.hasChildNodes())
        {
            if (leftElement.tagName() == rightElement.tagName())
            {
                equal = equal && domElementsEqual(leftElement, rightElement);
                found = true;
            }
        }
        else
        {
            if (atrributesEqual(leftElement, rightElement))
            {
                found = true;
                break;
            }
        }
    }

    return found && equal;
}

bool domNodeListsEqual(const QDomNodeList& leftNodes,
                       const QDomNodeList& rightNodes)
{
    for (int i{0}; i < leftNodes.size(); ++i)
    {
        const QDomElement leftElement{leftNodes.at(i).toElement()};
        if (!haveSameElement(rightNodes, leftElement))
            return false;
    }
    return true;
}

bool domElementsEqual(const QDomElement& left, const QDomElement& right)
{
    if (left.childNodes().count() != right.childNodes().count())
        return false;

    if (!left.hasChildNodes())
        return atrributesEqual(left, right);

    const QDomNodeList leftNodes{left.childNodes()};
    const QDomNodeList rightNodes{right.childNodes()};
    return domNodeListsEqual(leftNodes, rightNodes) &&
           domNodeListsEqual(rightNodes, leftNodes);
}

void saveExpectedDefinition(const QString& definition, const QString& filePath)
{
    Common::saveFile(filePath + Common::getDefinitionDumpSuffix(), definition);
}

void saveExpectedTsv(const QTableView& view, const QString& filePath)
{
    const QString tsvData{DatasetCommon::getExportedTsv(view)};
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
    const QDomElement leftRoot{leftDom.documentElement()};
    const QDomElement rightRoot{rightDom.documentElement()};

    return domElementsEqual(leftRoot, rightRoot);
}

static void checkDatasetDefinition(const QByteArray& definitionActual,
                                   const QString& expectedDefinitionFilePath)
{
    const QByteArray definitionExpected{
        file_utilities::loadFile(expectedDefinitionFilePath).second.toUtf8()};
    QVERIFY(xmlsAreEqual(definitionExpected, definitionActual));
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
    TableModel model(std::move(dataset));
    FilteringProxyModel proxyModel;
    proxyModel.setSourceModel(&model);

    QTableView view;
    view.setModel(&proxyModel);

    const QString actualData{getExportedTsv(view)};
    const QString expectedData{
        file_utilities::loadFile(filePath + Common::getDataTsvDumpSuffix())
            .second};

    static QRegularExpression splitRegexp{QStringLiteral("\n|\r\n")};
    QStringList actualDataLines{actualData.split(splitRegexp)};
    QStringList expectedDataLines{expectedData.split(splitRegexp)};
    QCOMPARE(actualDataLines.size(), expectedDataLines.size());
    for (int i{0}; i < actualDataLines.size(); ++i)
    {
        const QString& actualLine{actualDataLines.at(i)};
        const QString& expectedLine{expectedDataLines.at(i)};
        if (actualLine != expectedLine)
        {
            const QString msg{"Difference in line " + QString::number(i + 1) +
                              "\nActual:  " + actualLine +
                              "\nExpected: " + expectedLine};
            QFAIL(msg.toStdString().c_str());
        }
    }
}

std::unique_ptr<Dataset> createDataset(const QString& fileName,
                                       const QString& filePath)
{
    std::unique_ptr<Dataset> dataset{nullptr};
    if (fileName.endsWith(QStringLiteral(".xlsx")))
        dataset = std::make_unique<DatasetXlsx>(fileName, filePath);
    else if (fileName.endsWith(QStringLiteral(".ods")))
        dataset = std::make_unique<DatasetOds>(fileName, filePath);
    else
        dataset = std::make_unique<DatasetInner>(fileName);
    return dataset;
}

void activateAllDatasetColumns(Dataset& dataset)
{
    const QVector<bool> activeColumns(dataset.columnCount(), true);
    dataset.setActiveColumns(activeColumns);
}

void checkDefinition(const QString& fileName, const QString& dir)
{
    const QString filePath{dir + fileName};
    std::unique_ptr<Dataset> dataset{createDataset(fileName, filePath)};
    QVERIFY(dataset->initialize());
    QVERIFY(dataset->isValid());

    const QByteArray definition{dataset->definitionToXml(dataset->rowCount())};
    checkDatasetDefinition(definition,
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

    const QString filePath{destinationDir + fileName};
    const QString definition{
        QString::fromUtf8(dataset->definitionToXml(dataset->rowCount()))};
    saveExpectedDefinition(definition, filePath);

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
