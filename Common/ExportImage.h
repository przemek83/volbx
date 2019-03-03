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
    static void exportSingleImage(QwtPlotRenderer& rend, QList<PlotBase*> list);

    static void exportTwinImage(QList<PlotBase*> list, QwtPlotRenderer& rend);

    static void exportFourPlotsImage(QList<PlotBase*> list, QwtPlotRenderer& rend);
};

#endif // EXPORTIMAGE_H
