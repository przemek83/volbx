#include "PlotBase.h"

#include <QMouseEvent>
#include <qwt_text_label.h>

#include "Common/Constants.h"

#include "Zoomer.h"

PlotBase::PlotBase(const QString& title, QWidget* parent) :
    QwtPlot(/*title,*/ parent), panner_(canvas()), magnifier_(canvas())
{
    //Used in export of images.
    setWindowTitle(title);

    panner_.setAxisEnabled(QwtPlot::yLeft, true);
    panner_.setAxisEnabled(QwtPlot::xBottom, true);

    setStdScaleDraw(xBottom);
    setStdScaleDraw(yLeft);

    setAxisLabelRotation(QwtPlot::xBottom, Constants::DEFAULT_LABEL_ROTATION);
    setAxisLabelAlignment(QwtPlot::xBottom, Qt::AlignLeft | Qt::AlignBottom);

    initialScaleMap_.clear();
}

void PlotBase::mouseDoubleClickEvent(QMouseEvent* event)
{
    QwtPlot::mouseDoubleClickEvent(event);
    if (Qt::LeftButton == event->button())
    {
        resetPlot();
    }
}

void PlotBase::resetPlot()
{
    magnifier_.reset();
    for (int i = 0; i < QwtPlot::axisCnt; ++i)
    {
        if (initialScaleMap_.contains(i))
        {
            setAxisScale(i, initialScaleMap_[i].rx(), initialScaleMap_[i].ry());
        }
        else
        {
            setAxisAutoScale(i);
        }
    }
    replot();
}

void PlotBase::setStdScaleDraw(QwtPlot::Axis axis)
{
    setAxisScaleDraw(axis, new IntervalsScaleDraw());
}

void PlotBase::setAxisScale(int axisId, double min, double max, double step)
{
    initialScaleMap_.insert(axisId, QPointF(min, max));
    QwtPlot::setAxisScale(axisId, min, max, step);
}

QwtText PlotBase::IntervalsScaleDraw::label(double v) const
{
    if (!Constants::doublesAreEqual(fmod(v, 1), 0.))
    {
        return QwtText(Constants::floatToStringUsingLocale(static_cast<float>(v), 1));
    }

    return QwtText(Constants::floatToStringUsingLocale(static_cast<float>(v), 0));
}

void PlotBase::setPlotTitle(const QString& title)
{
    QwtText titleToSet = QwtText(title);
    QFont titleFont = titleToSet.font();
    titleFont.setPointSizeF(titleFont.pointSizeF() / 1);
    titleToSet.setFont(titleFont);
    setTitle(titleToSet);
}

QSize PlotBase::minimumSizeHint() const
{
    const int minimumWidth {100};
    const int minimumHeight {100};
    return QSize(minimumWidth, minimumHeight);
}
