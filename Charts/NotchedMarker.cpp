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
    auto widthAsDouble = static_cast<double>(width);
    //Center of x axis for item.
    double pointX = xMap.transform(elementNumber);

    //Draw horizontal line for max.
    p->setPen(QPen(Qt::DotLine));
    double yLevelMax = yMap.transform(static_cast<double>(quantiles.max_));
    p->drawLine(QPointF(pointX - widthAsDouble / 2., yLevelMax),
                QPointF(pointX + widthAsDouble / 2., yLevelMax));
    p->setPen(QPen(Qt::SolidLine));

    //Draw horizontal line for q90.
    double yLevelQ90 = yMap.transform(static_cast<double>(quantiles.q90_));
    p->drawLine(QPointF(pointX - widthAsDouble / 2., yLevelQ90),
                QPointF(pointX + widthAsDouble / 2., yLevelQ90));

    //Draw vertical line from q90 to q75.
    double yLevelQ75 = yMap.transform(static_cast<double>(quantiles.q75_));
    p->drawLine(QPointF(pointX, yLevelQ90), QPointF(pointX, yLevelQ75));

    //Draw center figure.
    double yLevelQ50 = yMap.transform(static_cast<double>(quantiles.q50_));
    double yLevelQ25 = yMap.transform(static_cast<double>(quantiles.q25_));
    double notchFactor = 0.85;
    QPainterPath path(QPoint(static_cast<int>(pointX - widthAsDouble), static_cast<int>(yLevelQ25)));
    path.lineTo(pointX - widthAsDouble,  yLevelQ25 - (yLevelQ25 - yLevelQ50) * notchFactor);
    path.lineTo(pointX - widthAsDouble / 2., yLevelQ50);
    path.lineTo(pointX - widthAsDouble, yLevelQ75 + (yLevelQ50 - yLevelQ75) * notchFactor);
    path.lineTo(pointX - widthAsDouble, yLevelQ75);
    path.lineTo(pointX + widthAsDouble, yLevelQ75);
    path.lineTo(pointX + widthAsDouble, yLevelQ75 + (yLevelQ50 - yLevelQ75) * notchFactor);
    path.lineTo(pointX + widthAsDouble / 2., yLevelQ50);
    path.lineTo(pointX + widthAsDouble, yLevelQ25 - (yLevelQ25 - yLevelQ50) * notchFactor);
    path.lineTo(pointX + widthAsDouble, yLevelQ25);
    path.closeSubpath();
    p->fillPath(path, markerBrush_);
    p->drawPath(path);

    //Draw q50.
    p->drawLine(QPointF(pointX - widthAsDouble / 2., yLevelQ50),
                QPointF(pointX + widthAsDouble / 2., yLevelQ50));

    //Draw vertical line from q25 to q10.
    double yLevelQ10 = yMap.transform(static_cast<double>(quantiles.q10_));
    p->drawLine(QPointF(pointX, yLevelQ25), QPointF(pointX, yLevelQ10));

    //Draw horizontal line for q10.
    p->drawLine(QPointF(pointX - widthAsDouble / 2., yLevelQ10),
                QPointF(pointX + widthAsDouble / 2., yLevelQ10));

    //Draw horizontal line for min.
    p->setPen(QPen(Qt::DotLine));
    double yLevelMin = yMap.transform(static_cast<double>(quantiles.min_));
    p->drawLine(QPointF(pointX - widthAsDouble / 2., yLevelMin),
                QPointF(pointX + widthAsDouble / 2., yLevelMin));
    p->setPen(QPen(Qt::SolidLine));

    //Draw avg cross.
    double yLevelAvg = yMap.transform(static_cast<double>(quantiles.avg_));
    p->drawLine(QPointF(pointX - widthAsDouble / 7.,
                        yLevelAvg - widthAsDouble / 7.),
                QPointF(pointX + widthAsDouble / 7.,
                        yLevelAvg + widthAsDouble / 7.));
    p->drawLine(QPointF(pointX + widthAsDouble / 7.,
                        yLevelAvg - widthAsDouble / 7.),
                QPointF(pointX - widthAsDouble / 7.,
                        yLevelAvg + widthAsDouble / 7.));
}

void NotchedMarker::drawLegend(QPainter* p, const QRectF& rect) const
{
    //Size of one part of legend.
    int sectionSize = 20;

    //Middle on x axis.
    double pointX = rect.x() + 11;

    //Left x, where legend starts.
    double leftX = rect.x() + 4;

    //Top y axis point from where legend starts.
    double topY = rect.y() + 8;

    //Place on x axis where legend text starts.
    double textStartX = rect.x() + 22;

    //Place on y axis where legend text starts.
    double textStartY = topY + 4;

    //Width of legent picture (except text).
    int width = 14;

    //Cross.
    p->drawLine(QPointF(leftX + 2, topY - 4), QPointF(leftX + 10, topY + 4));
    p->drawLine(QPointF(leftX + 2, topY + 4), QPointF(leftX + 10, topY - 4));
    p->drawText(QPointF(textStartX, textStartY), QObject::tr("mean"));

    //Max & Min.
    p->setPen(QPen(Qt::DotLine));
    p->drawLine(QPointF(leftX, topY + sectionSize), QPointF(leftX + width, topY + sectionSize));
    p->drawText(QPointF(textStartX, textStartY + sectionSize), QStringLiteral("max"));
    p->drawLine(QPointF(leftX, topY + 7 * sectionSize), QPointF(leftX + width, topY + 7 * sectionSize));
    p->drawText(QPointF(textStartX, textStartY + 7 * sectionSize),
                QStringLiteral("min"));
    p->setPen(QPen(Qt::SolidLine));

    //Q90.
    p->drawLine(QPointF(leftX + 2,
                        topY + 2 * sectionSize),
                QPointF(leftX + width - 2,
                        topY + 2 * sectionSize));
    p->drawText(QPointF(textStartX, textStartY + 2 * sectionSize), QStringLiteral("Q90"));

    //Vertical line from q90 to q75
    p->drawLine(QPointF(pointX, topY + 2 * sectionSize), QPointF(pointX, topY + 3 * sectionSize));

    //Main legend part.
    double notchFactor = 0.85;
    QPainterPath path(QPoint(static_cast<int>(leftX), static_cast<int>(topY + 5 * sectionSize)));
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
    p->drawText(QPointF(textStartX, textStartY + 3 * sectionSize), QStringLiteral("75"));

    //Q50.
    p->drawLine(QPointF(leftX + 4,
                        topY + 4 * sectionSize),
                QPointF(leftX + width - 4,
                        topY + 4 * sectionSize));
    p->drawText(QPointF(textStartX, textStartY + 4 * sectionSize),
                QStringLiteral("Q50"));

    //Q25.
    p->drawText(QPointF(textStartX, textStartY + 5 * sectionSize), QStringLiteral("Q25"));

    //Vertical line from q25 to q10.
    p->drawLine(QPointF(pointX, topY + 5 * sectionSize), QPointF(pointX, topY + 6 * sectionSize));

    //Q10.
    p->drawLine(QPointF(leftX + 2,
                        topY + 6 * sectionSize),
                QPointF(leftX + width - 2,
                        topY + 6 * sectionSize));
    p->drawText(QPointF(textStartX, textStartY + 6 * sectionSize), QStringLiteral("10"));
}
