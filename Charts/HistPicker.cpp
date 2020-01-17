#include "HistPicker.h"

HistPicker::HistPicker(QWidget* parent)
    : Picker(parent)
{

}

QwtText HistPicker::trackerTextF(const QPointF& pos)const
{
    QwtText coords(QString::number(pos.x(), 'f', 0) +
                   ", " + QString::number(pos.y(), 'f', 0));
    QColor bg(Qt::white);
    coords.setBackgroundBrush(QBrush(bg));
    return coords;
}
