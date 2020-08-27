#include "Common.h"

#include <QBuffer>
#include <QDomDocument>
#include <QFile>
#include <QTableView>
#include <QTextStream>
#include <QtTest/QtTest>

#include <Dataset.h>
#include <DatasetOds.h>
#include <DatasetXlsx.h>
#include <ExportDsv.h>
#include <FileUtilities.h>
#include <FilteringProxyModel.h>
#include <TableModel.h>

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
}  // namespace

namespace Common
{
void saveFile(const QString& fileName, const QString& data)
{
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);

    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    stream << data;
}

QString getDefinitionDumpSuffix()
{
    QString definitionDumpSuffix = "_DefinitionDump.txt";
    return definitionDumpSuffix;
}

QString getDefinitionRefSuffix()
{
    QString definitionDumpSuffix = "_DefinitionRef.txt";
    return definitionDumpSuffix;
}

QString getDataTsvDumpSuffix()
{
    QString dataTsvDumpSuffix = "_tsvDump.txt";
    return dataTsvDumpSuffix;
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

void compareExportDataWithDump(std::unique_ptr<Dataset> dataset)
{
    QString datasetName{dataset->getName()};
    TableModel model(std::move(dataset));
    FilteringProxyModel proxyModel;
    proxyModel.setSourceModel(&model);

    QTableView view;
    view.setModel(&proxyModel);

    QString actualData{Common::getExportedTsv(view)};
    QString expectedData{
        FileUtilities::loadFile(datasetName + Common::getDataTsvDumpSuffix())
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

std::unique_ptr<DatasetSpreadsheet> createDataset(const QString& filePath)
{
    std::unique_ptr<DatasetSpreadsheet> dataset{nullptr};
    if (filePath.endsWith(".xlsx"))
        dataset = std::make_unique<DatasetXlsx>(filePath, filePath);
    else
        dataset = std::make_unique<DatasetOds>(filePath, filePath);
    return dataset;
}

QString getSpreadsheetsDir()
{
    return QString(":/TestFiles/TestSpreadsheets/");
}

}  // namespace Common
