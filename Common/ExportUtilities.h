#ifndef EXPORTUTILITIES_H
#define EXPORTUTILITIES_H

#include <QString>

class QAbstractItemView;
class ProgressBarCounter;

namespace ExportUtilities
{
bool saveDataset(const QString& filePath, const QAbstractItemView* view,
                 ProgressBarCounter* bar);
}  // namespace ExportUtilities

#endif  // EXPORTUTILITIES_H
