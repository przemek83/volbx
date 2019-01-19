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
    static void exportAsImage(PlotBase* plot, QString fileName);

    static void quickExportAsImage(QList<PlotBase*> list);

private:
    ExportImage();
    virtual ~ExportImage();
    Q_DISABLE_COPY(ExportImage)

    static void exportSingleImage(QwtPlotRenderer& rend, QList<PlotBase*> list);

    static void exportTwinImage(QList<PlotBase*> list, QwtPlotRenderer& rend);

    static void exportFourPlotsImage(QList<PlotBase*> list, QwtPlotRenderer& rend);
};

#endif // EXPORTIMAGE_H
