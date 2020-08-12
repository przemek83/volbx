#include "Common.h"

#include <ExportDsv.h>
#include <QBuffer>
#include <QDomDocument>
#include <QFile>
#include <QTextStream>

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

}  // namespace Common
