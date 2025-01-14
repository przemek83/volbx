#pragma once

#include <QList>

class PlotBase;
class QString;

namespace ExportImage
{
void exportAsImage(PlotBase* plot, const QString& fileName);

void quickExportAsImage(const QList<PlotBase*>& list);
}  // namespace ExportImage
