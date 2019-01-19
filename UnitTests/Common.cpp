#include "Common.h"

#include <QFile>
#include <QTextStream>

QString Common::loadFile(QString name)
{
    QFile file(name);

    if( false == file.open(QIODevice::ReadOnly) )
    {
        return "";
    }

    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    return stream.readAll();
}

void Common::saveFile(QString fileName, QString& data)
{
    QFile file(fileName);

    file.open(QIODevice::WriteOnly);

    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    stream << data;

    file.close();
}

QString Common::getDefinitionDumpSuffix()
{
    static QString definitionDumpSuffix = "_DefinitionDump.txt";
    return definitionDumpSuffix;
}

QString Common::getDefinitionRefSuffix()
{
    static QString definitionDumpSuffix = "_DefinitionRef.txt";
    return definitionDumpSuffix;
}

QString Common::getDataTsvDumpSuffix()
{
    static QString dataTsvDumpSuffix = "_tsvDump.txt";
    return dataTsvDumpSuffix;
}

