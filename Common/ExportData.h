#ifndef EXPORTDATA_H
#define EXPORTDATA_H

#include <QString>

class QAbstractItemView;

namespace ExportData
{
bool asXLSX(const QAbstractItemView* view, const QString& fileName);

void quickAsTSV(const QAbstractItemView* view);

bool asCsv(const QAbstractItemView* view,
           const QString& fileName,
           bool innerFormat);

bool saveDataset(const QString& filePath, const QAbstractItemView* view);
}

#endif // EXPORTDATA_H
