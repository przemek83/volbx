#ifndef CUSTOMMARKER_H
#define CUSTOMMARKER_H

#include <qwt_plot_item.h>

struct Quantiles;

/**
 * @brief Base class for custom QwtPlotItems.
 */
class CustomMarker: public QwtPlotItem
{
public:
    explicit CustomMarker(QVector<Quantiles>* quantiles);

    ~CustomMarker() override = default;

    CustomMarker& operator=(const CustomMarker& other) = delete;
    CustomMarker(const CustomMarker& other) = delete;

    CustomMarker& operator=(CustomMarker&& other) = delete;
    CustomMarker(CustomMarker&& other) = delete;

    int rtti() const override;

    void draw(QPainter* p, const QwtScaleMap& xMap, const QwtScaleMap& yMap,
              const QRectF& rect) const override;

protected:
    virtual void drawLegend(QPainter* p,
                            const QRectF& rect) const = 0;

    virtual void drawElement(QPainter* p,
                             int elementNumber,
                             const QwtScaleMap& xMap,
                             const QwtScaleMap& yMap,
                             float width,
                             Quantiles& quantiles) const = 0;

    QVector<Quantiles>* quantiles_;
};

#endif // CUSTOMMARKER_H
