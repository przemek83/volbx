#ifndef EXPORTUTILITIES_H
#define EXPORTUTILITIES_H

#include <QString>

class QAbstractItemView;

namespace ExportUtilities
{
bool saveDataset(const QString& filePath, const QAbstractItemView* view);
}  // namespace ExportUtilities

#endif  // EXPORTUTILITIES_H
