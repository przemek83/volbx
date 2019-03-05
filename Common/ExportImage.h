#ifndef EXPORTIMAGE_H
#define EXPORTIMAGE_H

#include <QString>

class PlotBase;
class QwtPlotRenderer;

/**
 * @brief static methods used to export images taken from plots.
 */
class ExportImage
{
public:
    ExportImage() = delete;
    ~ExportImage() = delete;

    ExportImage& operator=(const ExportImage& other) = delete;
    ExportImage(const ExportImage& other) = delete;

    ExportImage& operator=(ExportImage&& other) = delete;
    ExportImage(ExportImage&& other) = delete;

    static void exportAsImage(PlotBase* plot, QString fileName);

    static void quickExportAsImage(QList<PlotBase*> list);

private:
    static void exportSingleImage(const QwtPlotRenderer& rend, const QList<PlotBase*>& list);

    static void exportTwinImage(const QList<PlotBase*>& list, const QwtPlotRenderer& rend);

    static void exportFourPlotsImage(const QList<PlotBase*>& list, const QwtPlotRenderer& rend);
};

#endif // EXPORTIMAGE_H
