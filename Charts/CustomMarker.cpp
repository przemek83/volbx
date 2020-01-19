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
    setZ(Constants::HIGH_ORDER);
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
    if (Constants::doublesAreEqual(scaleLeft.lowerBound(), scaleLeft.upperBound()))
    {
        return;
    }

    p->save();
    p->setBrush(QBrush(Qt::red, Qt::NoBrush));

    const int minWidthForLegend {100};
    if (quantiles_->size() == 1 && rect.size().width() > minWidthForLegend)
    {
        drawLegend(p, rect);
    }

    double width =
        xMap.pDist() / ((scaleBottom.upperBound() - scaleBottom.lowerBound()) * 2);

    //Item should take 90% of place.
    const double widthFactor {.9};
    width *= widthFactor;

    //Draw elements.
    int elementNumber = 0;
    for (Quantiles& quantiles : *quantiles_)
    {
        elementNumber++;
        drawElement(p, elementNumber, xMap, yMap, width, quantiles);
    }
    p->restore();
}
