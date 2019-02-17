#include <QDebug>
#include <qwt_plot_canvas.h>

#include "Common/Constants.h"

#include "Picker.h"

Picker::Picker(QWidget* parent)
    : QwtPlotPicker(parent), mouseInWidget_(false)
{
    setTrackerMode(AlwaysOn);

    QPen pen = trackerPen();
    pen.setColor(Qt::blue);
    pen.setWidth(2);
    setTrackerPen(pen);

    QFont actualFont = trackerFont();
    actualFont.setPointSize(actualFont.pointSize()*1.4);
    actualFont.setWeight(QFont::Bold);
    setTrackerFont(actualFont);
}

int Picker::getAreaOfMouse()
{
    int x = static_cast<int>(invTransform(trackerPosition()).x()+0.5);
    return x;
}

void Picker::widgetEnterEvent(QEvent* event)
{
    QwtPicker::widgetEnterEvent(event);
    mouseInWidget_ = true;
}

void Picker::widgetLeaveEvent(QEvent* event)
{
    QwtPicker::widgetLeaveEvent(event);
    mouseInWidget_ = false;
}

bool Picker::getMouseInWidget()
{
    return mouseInWidget_;
}
