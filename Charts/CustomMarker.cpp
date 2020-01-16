#include "CustomMarker.h"

#include <QVector>
#include <qpainter.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_div.h>
#include <qwt_scale_map.h>

#include "Common/Constants.h"
#include "Common/Quantiles.h"

#include "PlotBase.h"

CustomMarker::CustomMarker(QVector<Quantiles>* quantiles)
    : quantiles_(quantiles)
{
    setZ(1000);
    setRenderHint(QwtPlotItem::RenderAntialiased, true);
}

int CustomMarker::rtti() const
{
    return QwtPlotItem::Rtti_PlotUserItem;
}

void CustomMarker::draw(QPainter* p,
                        const QwtScaleMap& xMap,
                        const QwtScaleMap& yMap,
                        const QRectF& rect) const
{
    Q_ASSERT(nullptr != quantiles_);

    if (nullptr == quantiles_)
    {
        return;
    }

    auto basePlot = dynamic_cast<PlotBase*>(plot());
    const QwtScaleDiv& scaleLeft = basePlot->axisScaleDiv(QwtPlot::yLeft);
    const QwtScaleDiv& scaleBottom = basePlot->axisScaleDiv(QwtPlot::xBottom);

    //If max scale = min scale than do not draw.
    if (qFuzzyCompare(scaleLeft.lowerBound(), scaleLeft.upperBound()))
    {
        return;
    }

    int itemOnX = 0;
    p->save();
    p->setBrush(QBrush(Qt::red, Qt::NoBrush));

    if (quantiles_->size() == 1 && rect.size().width() > 100)
    {
        drawLegend(p, rect);
    }

    double width =
        xMap.pDist() / ((scaleBottom.upperBound() - scaleBottom.lowerBound()) * 2);

    //Item should take 90% of place.
    width *= 0.9;

    //Draw elements.
    for (Quantiles& quantiles : *quantiles_)
    {
        itemOnX++;

        //If min = max draw only one line.
        if (qFuzzyCompare(quantiles.min_, quantiles.max_))
        {
            double pointX = xMap.transform(itemOnX);
            double pointY = yMap.transform(static_cast<double>(quantiles.min_));
            p->drawLine(QPointF(pointX - width / 2., pointY),
                        QPointF(pointX + width / 2., pointY));
            continue;
        }

        drawElement(p, itemOnX, xMap, yMap, static_cast<float>(width), quantiles);
    }
    p->restore();
}
