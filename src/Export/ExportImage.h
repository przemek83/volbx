#pragma once

#include <QList>

class PlotBase;
class QString;

namespace export_image
{
void exportAsImage(PlotBase* plot, const QString& fileName);

void quickExportAsImage(const QList<PlotBase*>& list);
}  // namespace export_image
