#ifndef NOTCHEDMARKER_H
#define NOTCHEDMARKER_H

#include "CustomMarker.h"

struct Quantiles;

/**
 * @brief Notched marker for display of quantiles, min, max and avg in one item.
 */
class NotchedMarker : public CustomMarker
{
public:
    explicit NotchedMarker(QVector<Quantiles>* quantiles);

    ~NotchedMarker() override = default;

    NotchedMarker& operator=(const NotchedMarker& other) = delete;
    NotchedMarker(const NotchedMarker& other) = delete;

    NotchedMarker& operator=(NotchedMarker&& other) = delete;
    NotchedMarker(NotchedMarker&& other) = delete;

protected:
    void drawLegend(QPainter* p, const QRectF& rect) const override;

    void drawElement(QPainter* p, int elementNumber,
                     const QwtScaleMap& xMap, const QwtScaleMap& yMap,
                     float width, const Quantiles& quantiles) const override;

private:
    QBrush markerBrush_;
};

#endif // NOTCHEDMARKER_H
