#pragma once

#include <QList>

class PlotBase;
class QString;

/**
 * Functions for exporting images.
 */
namespace ExportImage
{
/**
 * @brief Expert given plot as image.
 * @param plot Plot to export.
 * @param fileName Name for image.
 */
void exportAsImage(PlotBase* plot, const QString& fileName);

/**
 * @brief Export images into clipboard.
 * @param list Plots to export.
 */
void quickExportAsImage(const QList<PlotBase*>& list);
}  // namespace ExportImage
