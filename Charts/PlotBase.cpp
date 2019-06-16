#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include <qwt_text_label.h>
#include <QMouseEvent>

#include "Common/Constants.h"

#include "PlotBase.h"
#include "Zoomer.h"

PlotBase::PlotBase(const QString& title, QWidget* parent) :
    QwtPlot(/*title,*/ parent)
{
    //Used in export of images.
    setWindowTitle(title);

    panner_ = new QwtPlotPanner(canvas());

    magnifier_ = new PlotMagnifier(canvas());

    panner_->setAxisEnabled(QwtPlot::yLeft, true);
    panner_->setAxisEnabled(QwtPlot::xBottom, true);

    setStdScaleDraw(xBottom);
    setStdScaleDraw(yLeft);

    setAxisLabelRotation(QwtPlot::xBottom, -50.0);
    setAxisLabelAlignment(QwtPlot::xBottom, Qt::AlignLeft | Qt::AlignBottom);

    initialScaleMap_.clear();
}

PlotBase::~PlotBase()
{
    delete panner_;
    delete magnifier_;
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
    magnifier_->reset();
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
    if (!qFuzzyCompare(fmod(v, 1), 0))
    {
        return QwtText(Constants::floatToStringUsingLocale(static_cast<float>(v), 1));
    }

    return QwtText(Constants::floatToStringUsingLocale(static_cast<float>(v), 0));
}

PlotBase::PlotMagnifier::PlotMagnifier(QWidget* canvas)
    : QwtPlotMagnifier(canvas)
{
    actualFactor_ = 1.0;
}

void PlotBase::PlotMagnifier::rescale(double factor)
{
    actualFactor_ *= factor;
    QwtPlotMagnifier::rescale(factor);
}

void PlotBase::PlotMagnifier::reset()
{
    rescale(1 / actualFactor_);
    actualFactor_ = 1.0;
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
    return QSize(100, 100);
}
