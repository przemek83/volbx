#ifndef NOTCHEDMARKER_H
#define NOTCHEDMARKER_H

#include "CustomMarker.h"

class Quantiles;

/**
 * @brief Notched marker for display of quantiles, min, max and avg in one item.
 */
class NotchedMarker : public CustomMarker
{
public:
    explicit NotchedMarker(QVector<Quantiles>* quantiles);

    ~NotchedMarker() override = default;

protected:
    void drawLegend(QPainter* p, const QRectF& rect) const override;

    void drawElement(QPainter* p, int elementNumber,
                     const QwtScaleMap& xMap, const QwtScaleMap& yMap,
                     float width, Quantiles& quantiles) const override;

private:
    Q_DISABLE_COPY(NotchedMarker)

    QBrush markerBrush_;
};

#endif // NOTCHEDMARKER_H
