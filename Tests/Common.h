#ifndef COMMON_H
#define COMMON_H

#include <QString>

class QAbstractItemView;

/**
 * @brief common functions for unit tests.
 */
namespace Common
{
QString loadFile(QString name);

void saveFile(QString fileName, const QString& data);

QString getDefinitionDumpSuffix();

QString getDefinitionRefSuffix();

QString getDataTsvDumpSuffix();

QString getExportedTsv(const QAbstractItemView& view);
};  // namespace Common

#endif  // COMMON_H
