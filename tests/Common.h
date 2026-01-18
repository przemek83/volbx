#pragma once

class QString;

namespace Common
{
void saveFile(const QString& fileName, const QString& data);

QString getDefinitionDumpSuffix();

QString getDataTsvDumpSuffix();

QString getSpreadsheetsDir();
}  // namespace Common
