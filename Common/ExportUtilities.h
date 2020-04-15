#ifndef EXPORTUTILITIES_H
#define EXPORTUTILITIES_H

#include <QString>

class QAbstractItemView;

namespace ExportUtilities
{
void quickAsTSV(const QAbstractItemView* view);

bool asCsv(const QAbstractItemView* view, const QString& fileName,
           bool innerFormat);

bool saveDataset(const QString& filePath, const QAbstractItemView* view);
}  // namespace ExportUtilities

#endif  // EXPORTUTILITIES_H
