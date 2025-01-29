#include "Common.h"

#include <QFile>
#include <QTextStream>
#include <QStringConverter>

namespace Common
{
void saveFile(const QString& fileName, const QString& data)
{
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);

    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    stream << data;
}

QString getDefinitionDumpSuffix()
{
    QString definitionDumpSuffix{QStringLiteral("_DefinitionDump.txt")};
    return definitionDumpSuffix;
}

QString getDataTsvDumpSuffix()
{
    QString dataTsvDumpSuffix{QStringLiteral("_tsvDump.txt")};
    return dataTsvDumpSuffix;
}

QString getSpreadsheetsDir()
{
    return QStringLiteral(":/TestFiles/TestSpreadsheets/");
}
}  // namespace Common
