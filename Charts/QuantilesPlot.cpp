#include <qwt_scale_div.h>
#include <QDebug>
#include <QResizeEvent>

#include "Common/Constants.h"

#include "NotchedMarker.h"
#include "QuantilesPlot.h"
#include "StringsScaleDraw.h"

QuantilesPlot::QuantilesPlot(QWidget* parent) :
    PlotBase(QObject::tr("Quantiles"), parent),
    marker_(nullptr)
{
    marker_ = new NotchedMarker(&quantiles_);

    picker_ = new BoxPicker(canvas());

    quantiles_.clear();

    enableAxis(yLeft, false);

    marker_->attach(this);

    //Font.
    QFont font = axisFont(xBottom);
    font.setStyleStrategy(QFont::PreferAntialias);
    setAxisFont(xBottom, font);

    setAxisScale(xBottom, 0, 2, 0);

    setAxisMaxMinor(xBottom, 0);
    setAxisMaxMajor(xBottom, 3);
}

QuantilesPlot::~QuantilesPlot()
{
    if( nullptr != picker_ )
        delete picker_;

    if( nullptr != marker_ )
        delete marker_;
}

void QuantilesPlot::forceResize()
{
    resizeEvent(new QResizeEvent(size(), size()));
}

void QuantilesPlot::setNewData(Quantiles quantiles)
{
    quantiles_.clear();
    quantiles_.append(quantiles);
    setAxisScale(QwtPlot::yLeft, quantiles.min_, quantiles.max_);

    setToolTip(quantiles.getValuesAsToolTip());

    setAxisScaleDraw(xBottom, new IntervalsScaleDraw(quantiles.number_));

    replot();
}

QSize QuantilesPlot::minimumSizeHint() const
{
    return QSize(50, 100);
}

QuantilesPlot::BoxPicker::BoxPicker(QWidget *parent)
    : Picker(parent)
{

}

QuantilesPlot::BoxPicker::~BoxPicker()
{

}

QwtText QuantilesPlot::BoxPicker::trackerTextF(const QPointF &pos)const
{
    QwtText coords(QString::number(pos.y(), 'f', 2));
    QColor bg(Qt::white);
    coords.setBackgroundBrush(QBrush(bg));
    return coords;
}

QuantilesPlot::IntervalsScaleDraw::IntervalsScaleDraw(int count) :
    QwtScaleDraw(), count_(count)
{

}

QuantilesPlot::IntervalsScaleDraw::~IntervalsScaleDraw()
{

}

QwtText QuantilesPlot::IntervalsScaleDraw::label(double v) const
{
    if( 1 == v && 0 != count_ )
        return QwtText(QString::number(count_));
    else
        return QwtText();
}
