#include "BoxPicker.h"

BoxPicker::BoxPicker(QWidget* parent)
    : Picker(parent)
{

}

QwtText BoxPicker::trackerTextF(const QPointF& pos)const
{
    QwtText coords(QString::number(pos.y(), 'f', 2));
    QColor bg(Qt::white);
    coords.setBackgroundBrush(QBrush(bg));
    return coords;
}
