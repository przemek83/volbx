#include "Common.h"

#include <QFile>
#include <QTextStream>

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

QString getSpreadsheetsDir()
{
    return QString(":/TestFiles/TestSpreadsheets/");
}
}  // namespace Common
