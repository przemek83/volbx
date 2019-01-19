#ifndef CUSTOMMARKER_H
#define CUSTOMMARKER_H

#include <qwt_plot_item.h>

class Quantiles;

/**
 * @brief Base class for custom QwtPlotItems.
 */
class CustomMarker: public QwtPlotItem
{
public:
    explicit CustomMarker(QVector<Quantiles>* quantiles);

    virtual ~CustomMarker();

    virtual int rtti() const;

    virtual void draw(QPainter *p,
                      const QwtScaleMap &xMap,
                      const QwtScaleMap &yMap,
                      const QRectF &rect) const;

protected:
    virtual void drawLegend(QPainter* p,
                            const QRectF& rect) const = 0;

    virtual void drawElement(QPainter *p,
                             int elementNumber,
                             const QwtScaleMap &xMap,
                             const QwtScaleMap &yMap,
                             float width,
                             Quantiles& quantiles) const = 0;

    QVector<Quantiles>* quantiles_;

protected:
    Q_DISABLE_COPY(CustomMarker)
};

#endif // CUSTOMMARKER_H
