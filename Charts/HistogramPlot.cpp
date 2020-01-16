#include "HistogramPlot.h"

#include <QDate>
#include <QMouseEvent>
#include <QToolTip>
#include <qwt_legend.h>
#include <qwt_legend_label.h>
#include <qwt_plot_panner.h>
#include <qwt_symbol.h>

#include "Common/Constants.h"

HistogramPlot::HistogramPlot(QWidget* parent) :
    PlotBase(QObject::tr("Histogram"), parent),
    picker_(canvas()), plotData_(nullptr, nullptr, 0)
{
    initHistogramPlot();

    initActualDensity();

    //Legend.
    initLegend();

    setLegendItemChecked(&histPlot_);
    setLegendItemChecked(&actualDensity_);
}

void HistogramPlot::initHistogramPlot()
{
    histPlot_.setStyle(QwtPlotHistogram::Columns);
    histPlot_.setZ(1000);
    histPlot_.setRenderHint(QwtPlotItem::RenderAntialiased, true);
    histPlot_.attach(this);
    histPlot_.setTitle(QObject::tr("Histogram"));
}

void HistogramPlot::initActualDensity()
{
    actualDensity_.setTitle(QObject::tr("Distribution"));
    actualDensity_.setStyle(QwtPlotCurve::Lines);
    actualDensity_.setCurveAttribute(QwtPlotCurve::Fitted);
    QPen pen = actualDensity_.pen();
    pen.setColor(QColor(Qt::blue));
    pen.setWidth(2);
    actualDensity_.setPen(pen);
    actualDensity_.setZ(1000);
    actualDensity_.setRenderHint(QwtPlotItem::RenderAntialiased, true);
    actualDensity_.attach(this);
}

void HistogramPlot::initLegend()
{
    auto legend = new QwtLegend();
    legend->setDefaultItemMode(QwtLegendData::Checkable);
    legend->setFrameStyle(QFrame::Box | QFrame::Sunken);
    connect(legend,
            SIGNAL(checked(QVariant, bool, int)),
            this,
            SLOT(legendItemChecked(QVariant, bool, int)));
    insertLegend(legend, QwtPlot::BottomLegend);
}

HistogramPlot::HistPicker::HistPicker(QWidget* parent)
    : Picker(parent)
{

}

QwtText HistogramPlot::HistPicker::trackerTextF(const QPointF& pos)const
{
    QwtText coords(QString::number(pos.x(), 'f', 0) +
                   ", " + QString::number(pos.y(), 'f', 0));
    QColor bg(Qt::white);
    coords.setBackgroundBrush(QBrush(bg));
    return coords;
}

void HistogramPlot::legendItemChecked(const QVariant& itemInfo, bool on, int /*index*/)
{
    QwtPlotItem* plotItem = infoToItem(itemInfo);
    if (plotItem != nullptr)
    {
        plotItem->setVisible(on);
        replot();
    }
}

void HistogramPlot::setLegendItemChecked(QwtPlotItem* plot)
{
    QWidget* legendWidget =
        qobject_cast<QwtLegend*>(legend())->legendWidget(itemToInfo(plot));

    if (legendWidget != nullptr)
    {
        auto legendLabel = dynamic_cast<QwtLegendLabel*>(legendWidget);
        if (nullptr != legendLabel)
        {
            legendLabel->setChecked(true);
        }
    }
}

void HistogramPlot::recompute(int intervalsCount)
{
    setToolTip(quantiles_.getValuesAsToolTip());
    int count = plotData_.getDataSize();
    const double* data = plotData_.getDataY();

    /* To normalize use formula:
       Z = (X - Mean) / stdDev */

    float min = quantiles_.min_;
    float max = quantiles_.max_;

    float step = (max - min) / static_cast<float>(intervalsCount);

    QVector<int> intervals(count);
    for (int i = 0; i < count; ++i)
    {
        int index = static_cast<int>((static_cast<float>(data[i]) - min) / step);
        if (index > count - 1)
        {
            index = count - 1;
        }
        if (index < 0)
        {
            index = 0;
        }
        intervals[index]++;
    }

    QVector< QwtIntervalSample > samples;
    QVector< QPointF > actualPoints;
    for (int i = 0; i < intervalsCount; ++i)
    {
        float x = min + step * i;
        samples.append(QwtIntervalSample(intervals[i], static_cast<double>(x), static_cast<double>(x + step)));
        actualPoints.append(QPointF(static_cast<double>(x + step / 2.f), intervals[i]));
    }

    histPlot_.setSamples(samples);
    actualDensity_.setSamples(actualPoints);

    replot();
}

void HistogramPlot::setNewData(const PlotData& plotData,
                               const Quantiles& quantiles,
                               int intervalsCount)
{
    plotData_ = plotData;
    quantiles_ = quantiles;

    recompute(intervalsCount);
}
