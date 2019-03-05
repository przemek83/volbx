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
    explicit PlotBase(QString /*title*/, QWidget* parent = nullptr);

    ~PlotBase() override;

    PlotBase& operator=(const PlotBase& other) = delete;
    PlotBase(const PlotBase& other) = delete;

    PlotBase& operator=(PlotBase&& other) = delete;
    PlotBase(PlotBase&& other) = delete;

    void resetPlot();

    void setAxisScale(int axisId, double min, double max, double step = 0);

    virtual void setNewData(PlotData plotData);

    QSize minimumSizeHint() const override;

protected:
    class PlotMagnifier : public QwtPlotMagnifier
    {
    public:
        PlotMagnifier(QWidget* canvas);

        ~PlotMagnifier() override = default;

        PlotMagnifier& operator=(const PlotMagnifier& other) = delete;
        PlotMagnifier(const PlotMagnifier& other) = delete;

        PlotMagnifier& operator=(PlotMagnifier&& other) = delete;
        PlotMagnifier(PlotMagnifier&& other) = delete;

        void reset();

    protected:
        virtual void rescale(double factor);

    private:
        double actualFactor_;
    };

    QwtPlotPanner* panner_;

    PlotMagnifier* magnifier_;

    void setStdScaleDraw(QwtPlot::Axis axis);

    void mouseDoubleClickEvent(QMouseEvent* event) override;

    QwtPlotCurve* plotCurve_;

    void setPlotTitle(QString title);

private:
    class IntervalsScaleDraw: public QwtScaleDraw
    {
    public:
        IntervalsScaleDraw() = default;

        ~IntervalsScaleDraw() override = default;

        IntervalsScaleDraw& operator=(const IntervalsScaleDraw& other) = delete;
        IntervalsScaleDraw(const IntervalsScaleDraw& other) = delete;

        IntervalsScaleDraw& operator=(IntervalsScaleDraw&& other) = delete;
        IntervalsScaleDraw(IntervalsScaleDraw&& other) = delete;

        QwtText label(double v) const override;
    };

    QMap<int, QPointF> initialScaleMap_;

    void clearDistData();

    PlotData plotData_;
};

#endif // PLOTBASE_H
