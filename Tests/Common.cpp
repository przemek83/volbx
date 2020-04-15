#include "Common.h"

#include <ExportDsv.h>
#include <QBuffer>
#include <QFile>
#include <QTextStream>

namespace Common
{
QString loadFile(QString name)
{
    QFile file(name);

    if (!file.open(QIODevice::ReadOnly))
        return "";

    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    return stream.readAll();
}

void saveFile(QString fileName, const QString& data)
{
    QFile file(fileName);

    file.open(QIODevice::WriteOnly);

    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    stream << data;

    file.close();
}

QString getDefinitionDumpSuffix()
{
    static QString definitionDumpSuffix = "_DefinitionDump.txt";
    return definitionDumpSuffix;
}

QString getDefinitionRefSuffix()
{
    static QString definitionDumpSuffix = "_DefinitionRef.txt";
    return definitionDumpSuffix;
}

QString getDataTsvDumpSuffix()
{
    static QString dataTsvDumpSuffix = "_tsvDump.txt";
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
}  // namespace Common
