#include <qwt_plot_panner.h>
#include <QMouseEvent>
#include <qwt_symbol.h>
#include <qwt_legend.h>
#include <QDate>
#include <QToolTip>
#include <qwt_legend_label.h>

#include "Common/Constants.h"

#include "HistogramPlot.h"

HistogramPlot::HistogramPlot(QWidget *parent) :
    PlotBase(QObject::tr("Histogram"), parent),
    plotData_(PlotData(NULL, NULL, 0))
{
    picker_ = new HistPicker(canvas());

    initHistogramPlot();

    initActualDensity();

    //Legend.
    initLegend();

    setLegendItemChecked(&histPlot_);
    setLegendItemChecked(&actualDensity_);
}

HistogramPlot::~HistogramPlot()
{
    if( NULL != picker_ )
        delete picker_;
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
    QwtLegend* legend = new QwtLegend();
    legend->setDefaultItemMode(QwtLegendData::Checkable);
    legend->setFrameStyle(QFrame::Box|QFrame::Sunken);
    connect(legend,
            SIGNAL(checked(QVariant,bool,int)),
            this,
            SLOT(legendItemChecked(QVariant,bool,int)));
    insertLegend(legend, QwtPlot::BottomLegend);
}

HistogramPlot::HistPicker::HistPicker(QWidget *parent)
    : Picker(parent)
{

}

HistogramPlot::HistPicker::~HistPicker()
{

}

QwtText HistogramPlot::HistPicker::trackerTextF(const QPointF &pos)const
{
    QwtText coords(QString::number(pos.x(), 'f', 0) +
                   ", " + QString::number(pos.y(), 'f', 0));
    QColor bg(Qt::white);
    coords.setBackgroundBrush(QBrush(bg));
    return coords;
}

void HistogramPlot::legendItemChecked(QVariant itemInfo, bool on, int /*index*/)
{
    QwtPlotItem *plotItem = infoToItem( itemInfo );
    if ( plotItem )
    {
        plotItem->setVisible( on );
        replot();
    }
}

void HistogramPlot::setLegendItemChecked(QwtPlotItem* plot)
{
    QWidget* legendWidget =
        static_cast<QwtLegend*>(legend())->legendWidget(itemToInfo(plot));

    if ( legendWidget != NULL )
    {
        QwtLegendLabel* legendLabel = static_cast<QwtLegendLabel*>(legendWidget);
        if( NULL != legendLabel )
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
    for( int i = 0; i < count; ++i )
    {
        int index = (data[i] - min) / step;
        if( index > count - 1 )
        {
            index = count - 1;
        }
        if( index < 0 )
        {
            index = 0;
        }
        intervals[index]++;
    }

    QVector< QwtIntervalSample > samples;
    QVector< QPointF > actualPoints;
    for( int i = 0; i < intervalsCount; ++i )
    {
        float x = min + step * i;
        samples.append(QwtIntervalSample(intervals[i], x, x + step));
        actualPoints.append(QPointF(x + step / 2.0, intervals[i]));
    }

    histPlot_.setSamples(samples);
    actualDensity_.setSamples(actualPoints);

    replot();
}

void HistogramPlot::setNewData(PlotData plotData,
                               Quantiles quantiles,
                               int intervalsCount)
{
    plotData_ = plotData;
    quantiles_ = quantiles;

    recompute(intervalsCount);
}
