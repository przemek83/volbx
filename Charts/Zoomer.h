#ifndef ZOOMER_H
#define ZOOMER_H

#include <qwt_plot_zoomer.h>
#include <qwt_plot_canvas.h>

/**
 * @brief Zoomer for interactions using mouse.
 */
class Zoomer: public QwtPlotZoomer
{
    Q_OBJECT
public:
    explicit Zoomer(QwtPlotCanvas* canvas);

    ~Zoomer() override = default;

    Zoomer& operator=(const Zoomer& other) = delete;
    Zoomer(const Zoomer& other) = delete;

    Zoomer& operator=(Zoomer&& other) = delete;
    Zoomer(Zoomer&& other) = delete;

    QwtText trackerTextF(const QPointF& pos) const override;

    void restart();

protected:
    void widgetWheelEvent(QWheelEvent* event) override;
};


#endif // ZOOMER_H
