#include "CustomMarker.h"

#include <qpainter.h>
#include <qwt_scale_map.h>
#include <qwt_scale_div.h>
#include <qwt_plot_curve.h>
#include <QVector>

#include "Common/Quantiles.h"
#include "Common/Constants.h"

#include "PlotBase.h"

CustomMarker::CustomMarker(QVector<Quantiles>* quantiles)
    : QwtPlotItem(),
      quantiles_(quantiles)
{
    setZ(1000);
    setRenderHint(QwtPlotItem::RenderAntialiased, true);
}

CustomMarker::~CustomMarker()
{

}

int CustomMarker::rtti() const
{
    return QwtPlotItem::Rtti_PlotUserItem;
}

void CustomMarker::draw(QPainter* p,
                        const QwtScaleMap &xMap,
                        const QwtScaleMap &yMap,
                        const QRectF& rect) const
{
    Q_ASSERT(nullptr != quantiles_);

    if(nullptr == quantiles_)
        return;

    PlotBase* basePlot = dynamic_cast<PlotBase*>(plot());
    QwtScaleDiv scaleLeft = basePlot->axisScaleDiv(QwtPlot::yLeft);
    QwtScaleDiv scaleBottom = basePlot->axisScaleDiv(QwtPlot::xBottom);

    //If max scale = min scale than do not draw.
    if(scaleLeft.lowerBound() == scaleLeft.upperBound())
    {
        return;
    }

    int itemOnX = 0;
    p->save();
    p->setBrush(QBrush(Qt::red, Qt::NoBrush));

    if(quantiles_->size() == 1 && rect.size().width() > 100 )
    {
        drawLegend(p, rect);
    }

    float width =
        xMap.pDist() / ((scaleBottom.upperBound() - scaleBottom.lowerBound()) * 2);

    //Item should take 90% of place.
    width *= 0.9;

    //Draw elements.
    foreach(Quantiles quantiles, *quantiles_)
    {
        itemOnX++;

        //If min = max draw only one line.
        if(quantiles.min_ == quantiles.max_)
        {
            float pointX = xMap.transform(itemOnX);
            float pointY = yMap.transform(quantiles.min_);
            p->drawLine(pointX - width/2, pointY, pointX + width/2, pointY);
            continue;
        }

        drawElement(p, itemOnX, xMap, yMap, width, quantiles);
    }
    p->restore();
}
