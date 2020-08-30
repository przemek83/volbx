#pragma once

class QString;

/**
 * @brief Common functions for unit tests.
 */
namespace Common
{
void saveFile(const QString& fileName, const QString& data);

QString getDefinitionDumpSuffix();

QString getDefinitionRefSuffix();

QString getDataTsvDumpSuffix();

QString getSpreadsheetsDir();
};  // namespace Common
