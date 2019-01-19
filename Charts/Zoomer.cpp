#include <QWheelEvent>

#include "Common/Constants.h"

#include "Zoomer.h"

Zoomer::Zoomer(QwtPlotCanvas *canvas):
    QwtPlotZoomer(canvas)
{
    setTrackerMode(AlwaysOn);

    // LeftButton for the zooming
    // MidButton for the panning
    // RightButton: zoom out by 1
    // Ctrl+RighButton: zoom out to full size

    setMousePattern(QwtEventPattern::MouseSelect2,
            Qt::RightButton, Qt::ControlModifier);

    setMousePattern(QwtEventPattern::MouseSelect3,
            Qt::RightButton);

    const QColor c(Qt::darkBlue);
    setRubberBandPen(c);
    setTrackerPen(c);
}

Zoomer::~Zoomer()
{

}

QwtText Zoomer::trackerTextF(const QPointF &pos) const
{
    QColor bg(Qt::white);
    bg.setAlpha(200);

    QwtText text = QwtPlotZoomer::trackerTextF(pos);
    text.setBackgroundBrush( QBrush( bg ));
    return text;
}

void Zoomer::widgetWheelEvent(QWheelEvent* event)
{
    zoom((event->delta() > 0 ? 1 : -1));
    event->accept();
}

void Zoomer::restart()
{
    reset();
}
