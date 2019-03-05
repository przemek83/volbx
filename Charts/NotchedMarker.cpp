#include "NotchedMarker.h"

#include <qpainter.h>
#include <qwt_scale_map.h>
#include <qwt_scale_div.h>
#include <qwt_plot_curve.h>
#include <QVector>

#include "Common/Quantiles.h"
#include "Common/Constants.h"

#include "NotchedMarker.h"
#include "PlotBase.h"

NotchedMarker::NotchedMarker(QVector<Quantiles>* quantiles)
    : CustomMarker(quantiles)
{
    QLinearGradient gradient;
    gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    QColor whiteAlpha(Qt::white);
    whiteAlpha.setAlpha(200);
    QColor endAlpha(Qt::black);
    endAlpha.setAlpha(50);
    gradient.setColorAt(0, whiteAlpha);
    gradient.setColorAt(1, endAlpha);
    markerBrush_ = QBrush(gradient);
}

void NotchedMarker::drawElement(QPainter* p,
                                int elementNumber,
                                const QwtScaleMap& xMap,
                                const QwtScaleMap& yMap,
                                float width,
                                const Quantiles& quantiles) const
{
    //Center of x axis for item.
    float pointX = xMap.transform(elementNumber);

    //Draw horizontal line for max.
    p->setPen(QPen(Qt::DotLine));
    float yLevelMax = yMap.transform(quantiles.max_);
    p->drawLine(pointX - width / 2, yLevelMax, pointX + width / 2, yLevelMax);
    p->setPen(QPen(Qt::SolidLine));

    //Draw horizontal line for q90.
    float yLevelQ90 = yMap.transform(quantiles.q90_);
    p->drawLine(pointX - width / 2, yLevelQ90, pointX + width / 2, yLevelQ90);

    //Draw vertical line from q90 to q75.
    float yLevelQ75 = yMap.transform(quantiles.q75_);
    p->drawLine(pointX, yLevelQ90, pointX, yLevelQ75);

    //Draw center figure.
    float yLevelQ50 = yMap.transform(quantiles.q50_);
    float yLevelQ25 = yMap.transform(quantiles.q25_);
    float notchFactor = 0.85;
    QPainterPath path(QPoint(pointX - width, yLevelQ25));
    path.lineTo(pointX - width,  yLevelQ25 - (yLevelQ25 - yLevelQ50) * notchFactor);
    path.lineTo(pointX - width / 2, yLevelQ50);
    path.lineTo(pointX - width, yLevelQ75 + (yLevelQ50 - yLevelQ75) * notchFactor);
    path.lineTo(pointX - width, yLevelQ75);
    path.lineTo(pointX + width, yLevelQ75);
    path.lineTo(pointX + width, yLevelQ75 + (yLevelQ50 - yLevelQ75) * notchFactor);
    path.lineTo(pointX + width / 2, yLevelQ50);
    path.lineTo(pointX + width, yLevelQ25 - (yLevelQ25 - yLevelQ50) * notchFactor);
    path.lineTo(pointX + width, yLevelQ25);
    path.closeSubpath();
    p->fillPath(path, markerBrush_);
    p->drawPath(path);

    //Draw q50.
    p->drawLine(pointX - width / 2, yLevelQ50, pointX + width / 2, yLevelQ50);

    //Draw vertical line from q25 to q10.
    float yLevelQ10 = yMap.transform(quantiles.q10_);
    p->drawLine(pointX, yLevelQ25, pointX, yLevelQ10);

    //Draw horizontal line for q10.
    p->drawLine(pointX - width / 2, yLevelQ10, pointX + width / 2, yLevelQ10);

    //Draw horizontal line for min.
    p->setPen(QPen(Qt::DotLine));
    float yLevelMin = yMap.transform(quantiles.min_);
    p->drawLine(pointX - width / 2, yLevelMin, pointX + width / 2, yLevelMin);
    p->setPen(QPen(Qt::SolidLine));

    //Draw avg cross.
    float yLevelAvg = yMap.transform(quantiles.avg_);
    p->drawLine(pointX - width / 7,
                yLevelAvg - width / 7,
                pointX + width / 7,
                yLevelAvg + width / 7);
    p->drawLine(pointX + width / 7,
                yLevelAvg - width / 7,
                pointX - width / 7,
                yLevelAvg + width / 7);
}

void NotchedMarker::drawLegend(QPainter* p, const QRectF& rect) const
{
    //Size of one part of legend.
    int sectionSize = 20;

    //Middle on x axis.
    int pointX = rect.x() + 11;

    //Left x, where legend starts.
    int leftX = rect.x() + 4;

    //Top y axis point from where legend starts.
    int topY = rect.y() + 8;

    //Place on x axis where legend text starts.
    int textStartX = rect.x() + 22;

    //Place on y axis where legend text starts.
    int textStartY = topY + 4;

    //Width of legent picture (except text).
    int width = 14;

    //Cross.
    p->drawLine(leftX + 2, topY - 4, leftX + 10, topY + 4);
    p->drawLine(leftX + 2, topY + 4, leftX + 10, topY - 4);
    p->drawText(textStartX, textStartY, QObject::tr("mean"));

    //Max & Min.
    p->setPen(QPen(Qt::DotLine));
    p->drawLine(leftX, topY + sectionSize, leftX + width, topY + sectionSize);
    p->drawText(textStartX, textStartY + sectionSize, "max");
    p->drawLine(leftX, topY + 7 * sectionSize, leftX + width, topY + 7 * sectionSize);
    p->drawText(textStartX, textStartY + 7 * sectionSize, "min");
    p->setPen(QPen(Qt::SolidLine));

    //Q90.
    p->drawLine(leftX + 2,
                topY + 2 * sectionSize,
                leftX + width - 2,
                topY + 2 * sectionSize);
    p->drawText(textStartX, textStartY + 2 * sectionSize, "Q90");

    //Vertical line from q90 to q75
    p->drawLine(pointX, topY + 2 * sectionSize, pointX, topY + 3 * sectionSize);

    //Main legend part.
    float notchFactor = 0.85;
    QPainterPath path(QPoint(leftX, topY + 5 * sectionSize));
    path.lineTo(leftX,  topY + 5 * sectionSize - sectionSize * notchFactor);
    path.lineTo(leftX + 4, topY + 4 * sectionSize);
    path.lineTo(leftX, topY + 3 * sectionSize + sectionSize * notchFactor);
    path.lineTo(leftX, topY + 3 * sectionSize);
    path.lineTo(leftX + width, topY + 3 * sectionSize);
    path.lineTo(leftX + width, topY + 3 * sectionSize + sectionSize * notchFactor);
    path.lineTo(leftX + width - 4, topY + 4 * sectionSize);
    path.lineTo(leftX + width, topY + 5 * sectionSize - sectionSize * notchFactor);
    path.lineTo(leftX + width, topY + 5 * sectionSize);
    path.closeSubpath();
    p->fillPath(path, markerBrush_);
    p->drawPath(path);

    //Q75.
    p->drawText(textStartX, textStartY + 3 * sectionSize, "Q75");

    //Q50.
    p->drawLine(leftX + 4,
                topY + 4 * sectionSize,
                leftX + width - 4,
                topY + 4 * sectionSize);
    p->drawText(textStartX, textStartY + 4 * sectionSize, "Q50");

    //Q25.
    p->drawText(textStartX, textStartY + 5 * sectionSize, "Q25");

    //Vertical line from q25 to q10.
    p->drawLine(pointX, topY + 5 * sectionSize, pointX, topY + 6 * sectionSize);

    //Q10.
    p->drawLine(leftX + 2,
                topY + 6 * sectionSize,
                leftX + width - 2,
                topY + 6 * sectionSize);
    p->drawText(textStartX, textStartY + 6 * sectionSize, "Q10");
}
