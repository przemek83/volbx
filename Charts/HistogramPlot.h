#ifndef HISTOGRAMPLOT_H
#define HISTOGRAMPLOT_H

#include <qwt_plot.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_picker.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_histogram.h>

#include "Common/Quantiles.h"
#include "Common/PlotData.h"

#include "PlotBase.h"
#include "Picker.h"

class QMouseEvent;

/**
 * @brief Histogram plot with actual density/distriburtion.
 */
class HistogramPlot : public PlotBase
{
    Q_OBJECT
public:
    explicit HistogramPlot(QWidget* parent = nullptr);

    ~HistogramPlot() override;

    HistogramPlot& operator=(const HistogramPlot& other) = delete;
    HistogramPlot(const HistogramPlot& other) = delete;

    HistogramPlot& operator=(HistogramPlot&& other) = delete;
    HistogramPlot(HistogramPlot&& other) = delete;

    void setNewData(PlotData plotData,
                    Quantiles quantiles,
                    int intervalsCount);

private:
    void initHistogramPlot();

    void initActualDensity();

    void initLegend();

    ///Histogram plot.
    QwtPlotHistogram histPlot_;

    ///Actual density plot (blue line).
    QwtPlotCurve actualDensity_;

    class HistPicker : Picker
    {
    public:
        explicit HistPicker(QWidget* parent);

        ~HistPicker() override = default;

        HistPicker& operator=(const HistPicker& other) = delete;
        HistPicker(const HistPicker& other) = delete;

        HistPicker& operator=(HistPicker&& other) = delete;
        HistPicker(HistPicker&& other) = delete;

    protected:
        QwtText trackerTextF(const QPointF& pos) const override;
    };

    HistPicker* picker_;

    void setLegendItemChecked(QwtPlotItem* plot);

    PlotData plotData_;

    Quantiles quantiles_;

private Q_SLOTS:
    void legendItemChecked(QVariant itemInfo, bool on, int index);

    void recompute(int intervalsCount);
};

#endif // HISTOGRAMPLOT_H
