#pragma once

#include <QString>

class QAbstractItemView;

/**
 * @brief common functions for unit tests.
 */
namespace Common
{
void saveFile(QString fileName, const QString& data);

QString getDefinitionDumpSuffix();

QString getDefinitionRefSuffix();

QString getDataTsvDumpSuffix();

QString getExportedTsv(const QAbstractItemView& view);

bool xmlsAreEqual(const QByteArray& left, const QByteArray& right);
};  // namespace Common
