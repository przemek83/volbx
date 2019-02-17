#ifndef PLOTBASE_H
#define PLOTBASE_H

#include <QPointF>
#include <qwt_plot.h>
#include <qwt_plot_magnifier.h>
#include <qwt_scale_div.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_canvas.h>

#include "Common/PlotData.h"

class Zoomer;
class QwtPlotPanner;

/**
 * @brief PlotBase class for all plots.
 */
class PlotBase : public QwtPlot
{
public:
    PlotBase(QString /*title*/, QWidget *parent = nullptr);

    virtual ~PlotBase();

    void resetPlot();

    void setAxisScale (int axisId, double min, double max, double step = 0);

    virtual void setNewData(PlotData plotData);

    virtual QSize minimumSizeHint() const;

protected:
    class PlotMagnifier : public QwtPlotMagnifier
    {
    public:
        PlotMagnifier(QWidget* canvas);

        virtual ~PlotMagnifier() = default;

        void reset();

    protected:
        virtual void rescale (double factor);

    private:
        double actualFactor_;
    };

    QwtPlotPanner* panner_;

    PlotMagnifier* magnifier_;

    void setStdScaleDraw(QwtPlot::Axis axis);

    virtual void mouseDoubleClickEvent(QMouseEvent* event);

    QwtPlotCurve* plotCurve_;

    void setPlotTitle(QString title);

private:
    Q_DISABLE_COPY(PlotBase)

    class IntervalsScaleDraw: public QwtScaleDraw
    {
    public:
        IntervalsScaleDraw();

        virtual ~IntervalsScaleDraw() = default;

        virtual QwtText label(double v) const;
    };

    QMap<int, QPointF> initialScaleMap_;

    void clearDistData();

    PlotData plotData_;
};

#endif // PLOTBASE_H
