#ifndef EXPORTDATA_H
#define EXPORTDATA_H

#include <QString>
#include <QVariant>
#include <vector>
#include <quazip5/quazipfile.h>

#include "Common/Formats.h"
#include "Common/SpecialColumns.h"

class QAbstractItemView;

namespace ExportData
{
bool asXLSX(const QAbstractItemView* view, const QString& fileName);

void quickAsTSV(const QAbstractItemView* view);

bool asCsv(const QAbstractItemView* view,
           const QString& fileName,
           bool innerFormat);

bool saveDataset(const QString& name, const QAbstractItemView* view);
}

#endif // EXPORTDATA_H
