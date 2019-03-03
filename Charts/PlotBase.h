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

    ~PlotBase() override;

    void resetPlot();

    void setAxisScale (int axisId, double min, double max, double step = 0);

    virtual void setNewData(PlotData plotData);

    QSize minimumSizeHint() const override;

protected:
    class PlotMagnifier : public QwtPlotMagnifier
    {
    public:
        PlotMagnifier(QWidget* canvas);

        ~PlotMagnifier() override = default;

        void reset();

    protected:
        virtual void rescale (double factor);

    private:
        double actualFactor_;
    };

    QwtPlotPanner* panner_;

    PlotMagnifier* magnifier_;

    void setStdScaleDraw(QwtPlot::Axis axis);

    void mouseDoubleClickEvent(QMouseEvent *event) override;

    QwtPlotCurve* plotCurve_;

    void setPlotTitle(QString title);

private:
    Q_DISABLE_COPY(PlotBase)

    class IntervalsScaleDraw: public QwtScaleDraw
    {
    public:
        IntervalsScaleDraw();

        ~IntervalsScaleDraw() override = default;

        QwtText label(double v) const override;
    };

    QMap<int, QPointF> initialScaleMap_;

    void clearDistData();

    PlotData plotData_;
};

#endif // PLOTBASE_H
