#ifndef EXPORTDATA_H
#define EXPORTDATA_H

#include <QString>

class QAbstractItemView;

namespace ExportData
{
void quickAsTSV(const QAbstractItemView* view);

bool asCsv(const QAbstractItemView* view, const QString& fileName,
           bool innerFormat);

bool saveDataset(const QString& filePath, const QAbstractItemView* view);
}  // namespace ExportData

#endif  // EXPORTDATA_H
