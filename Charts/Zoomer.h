#ifndef ZOOMER_H
#define ZOOMER_H

#include <qwt_plot_zoomer.h>
#include <qwt_plot_canvas.h>

/**
 * @brief Zoomer for interactions using mouse.
 */
class Zoomer: public QwtPlotZoomer
{
public:
    explicit Zoomer(QwtPlotCanvas *canvas);

    virtual ~Zoomer();

    virtual QwtText trackerTextF(const QPointF &pos) const;

    void restart();

protected:
    virtual void widgetWheelEvent(QWheelEvent* event);

private:
    Q_DISABLE_COPY(Zoomer)
};


#endif // ZOOMER_H
