#include "NotchedMarker.h"

#include <QVector>
#include <qpainter.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_div.h>
#include <qwt_scale_map.h>

#include "Common/Constants.h"
#include "Common/Quantiles.h"

#include "PlotBase.h"

NotchedMarker::NotchedMarker(QVector<Quantiles>* quantiles)
    : CustomMarker(quantiles)
{
    QLinearGradient gradient;
    gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    QColor whiteAlpha(Qt::white);
    whiteAlpha.setAlpha(Constants::SMALL_TRANSPARENCY_FACTOR);
    QColor endAlpha(Qt::black);
    endAlpha.setAlpha(Constants::BIG_TRANSPARENCY_FACTOR);
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
    const auto widthAsDouble = static_cast<double>(width);

    //Center of x axis for item.
    const double centerX = xMap.transform(elementNumber);
    const double leftEdgeX {centerX - widthAsDouble};
    const double rightEdgeX {centerX + widthAsDouble};
    const double middleOfLeftPartX {centerX - widthAsDouble / 2};
    const double middleOfRightPartX {centerX + widthAsDouble / 2};

    //Draw horizontal line for max.
    p->setPen(QPen(Qt::DotLine));
    const double yLevelMax = yMap.transform(static_cast<double>(quantiles.max_));
    p->drawLine(QPointF(middleOfLeftPartX, yLevelMax),
                QPointF(middleOfRightPartX, yLevelMax));

    p->setPen(QPen(Qt::SolidLine));
    //Draw horizontal line for q90.
    const double yLevelQ90 = yMap.transform(static_cast<double>(quantiles.q90_));
    p->drawLine(QPointF(middleOfLeftPartX, yLevelQ90),
                QPointF(middleOfRightPartX, yLevelQ90));

    //Draw vertical line from q90 to q75.
    const double yLevelQ75 = yMap.transform(static_cast<double>(quantiles.q75_));
    p->drawLine(QPointF(centerX, yLevelQ90), QPointF(centerX, yLevelQ75));

    //Draw center figure.
    const double yLevelQ50 = yMap.transform(static_cast<double>(quantiles.q50_));
    const double yLevelQ25 = yMap.transform(static_cast<double>(quantiles.q25_));
    const double notchFactor = 0.85;
    QPainterPath path(QPoint(static_cast<int>(leftEdgeX), static_cast<int>(yLevelQ25)));
    path.lineTo(leftEdgeX,  yLevelQ25 - (yLevelQ25 - yLevelQ50) * notchFactor);
    path.lineTo(middleOfLeftPartX, yLevelQ50);
    path.lineTo(leftEdgeX, yLevelQ75 + (yLevelQ50 - yLevelQ75) * notchFactor);
    path.lineTo(leftEdgeX, yLevelQ75);
    path.lineTo(rightEdgeX, yLevelQ75);
    path.lineTo(rightEdgeX, yLevelQ75 + (yLevelQ50 - yLevelQ75) * notchFactor);
    path.lineTo(middleOfRightPartX, yLevelQ50);
    path.lineTo(rightEdgeX, yLevelQ25 - (yLevelQ25 - yLevelQ50) * notchFactor);
    path.lineTo(rightEdgeX, yLevelQ25);
    path.closeSubpath();
    p->fillPath(path, markerBrush_);
    p->drawPath(path);

    //Draw q50.
    p->drawLine(QPointF(middleOfLeftPartX, yLevelQ50),
                QPointF(middleOfRightPartX, yLevelQ50));

    //Draw vertical line from q25 to q10.
    const double yLevelQ10 = yMap.transform(static_cast<double>(quantiles.q10_));
    p->drawLine(QPointF(centerX, yLevelQ25), QPointF(centerX, yLevelQ10));

    //Draw horizontal line for q10.
    p->drawLine(QPointF(middleOfLeftPartX, yLevelQ10),
                QPointF(middleOfRightPartX, yLevelQ10));

    //Draw horizontal line for min.
    p->setPen(QPen(Qt::DotLine));
    const double yLevelMin = yMap.transform(static_cast<double>(quantiles.min_));
    p->drawLine(QPointF(middleOfLeftPartX, yLevelMin),
                QPointF(middleOfRightPartX, yLevelMin));
    p->setPen(QPen(Qt::SolidLine));

    //Draw avg cross.
    const double yLevelAvg = yMap.transform(static_cast<double>(quantiles.avg_));
    const double crossWidth {widthAsDouble / 7.};
    p->drawLine(QPointF(centerX - crossWidth, yLevelAvg - crossWidth),
                QPointF(centerX + crossWidth, yLevelAvg + crossWidth));
    p->drawLine(QPointF(centerX + crossWidth, yLevelAvg - crossWidth),
                QPointF(centerX - crossWidth, yLevelAvg + crossWidth));
}

void NotchedMarker::drawLegend(QPainter* p, const QRectF& rect) const
{
    //Size of one part of legend.
    const int sectionSize = 20;

    //Middle on x axis.
    const double pointX = rect.x() + 11;

    //Left x, where legend starts.
    const double leftX = rect.x() + 4;

    //Top y axis point from where legend starts.
    const double topY = rect.y() + 8;

    //Place on x axis where legend text starts.
    const double textStartX = rect.x() + 22;

    //Place on y axis where legend text starts.
    const double textStartY = topY + 4;

    //Width of legent picture (except text).
    const int width = 14;

    //Cross.
    const double crossWidth {8};
    p->drawLine(QPointF(leftX + 2, topY - crossWidth / 2),
                QPointF(leftX + crossWidth, topY + crossWidth / 2));
    p->drawLine(QPointF(leftX + 2, topY + crossWidth / 2),
                QPointF(leftX + crossWidth, topY - crossWidth / 2));
    p->drawText(QPointF(textStartX, textStartY), QObject::tr("mean"));

    const int yLevelMax {sectionSize};
    p->setPen(QPen(Qt::DotLine));
    p->drawLine(QPointF(leftX, topY + yLevelMax), QPointF(leftX + width, topY + yLevelMax));
    p->drawText(QPointF(textStartX, textStartY + yLevelMax), QStringLiteral("max"));

    p->setPen(QPen(Qt::SolidLine));

    //Q90.
    const int yLevelQ90 {2 * sectionSize};
    const int minMaxIndent {2};
    p->drawLine(QPointF(leftX + minMaxIndent, topY + yLevelQ90),
                QPointF(leftX + width - minMaxIndent, topY + yLevelQ90));
    p->drawText(QPointF(textStartX, textStartY + yLevelQ90), QStringLiteral("Q90"));

    //Vertical line from q90 to q75
    const int yLevelQ75 {3 * sectionSize};
    const int yLevelQ50 {4 * sectionSize};
    const int yLevelQ25 {5 * sectionSize};
    p->drawLine(QPointF(pointX, topY + yLevelQ90), QPointF(pointX, topY + yLevelQ75));

    const int middleIndent {4};

    //Main legend part.
    const double notchFactor = 0.85;
    QPainterPath path(QPoint(static_cast<int>(leftX), static_cast<int>(topY + yLevelQ25)));
    path.lineTo(leftX,  topY + yLevelQ25 - sectionSize * notchFactor);
    path.lineTo(leftX + middleIndent, topY + yLevelQ50);
    path.lineTo(leftX, topY + yLevelQ75 + sectionSize * notchFactor);
    path.lineTo(leftX, topY + yLevelQ75);
    path.lineTo(leftX + width, topY + yLevelQ75);
    path.lineTo(leftX + width, topY + yLevelQ75 + sectionSize * notchFactor);
    path.lineTo(leftX + width - middleIndent, topY + yLevelQ50);
    path.lineTo(leftX + width, topY + yLevelQ25 - sectionSize * notchFactor);
    path.lineTo(leftX + width, topY + yLevelQ25);
    path.closeSubpath();
    p->fillPath(path, markerBrush_);
    p->drawPath(path);

    //Q75.
    p->drawText(QPointF(textStartX, textStartY + yLevelQ75), QStringLiteral("Q75"));

    //Q50.
    p->drawLine(QPointF(leftX + middleIndent, topY + yLevelQ50),
                QPointF(leftX + width - middleIndent, topY + yLevelQ50));
    p->drawText(QPointF(textStartX, textStartY + yLevelQ50), QStringLiteral("Q50"));

    //Q25.
    p->drawText(QPointF(textStartX, textStartY + yLevelQ25), QStringLiteral("Q25"));

    //Vertical line from q25 to q10.
    const int yLevelQ10 {6 * sectionSize};
    p->drawLine(QPointF(pointX, topY + yLevelQ25), QPointF(pointX, topY + yLevelQ10));

    //Q10.
    p->drawLine(QPointF(leftX + minMaxIndent, topY + yLevelQ10),
                QPointF(leftX + width - minMaxIndent, topY + yLevelQ10));
    p->drawText(QPointF(textStartX, textStartY + yLevelQ10), QStringLiteral("Q10"));

    p->setPen(QPen(Qt::DotLine));
    const int yLevelMin {7 * sectionSize};
    p->drawLine(QPointF(leftX, topY + yLevelMin), QPointF(leftX + width, topY + yLevelMin));
    p->drawText(QPointF(textStartX, textStartY + yLevelMin), QStringLiteral("min"));
}
