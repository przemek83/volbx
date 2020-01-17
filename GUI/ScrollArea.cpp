#include "ScrollArea.h"

#include "QResizeEvent"

ScrollArea::ScrollArea(QWidget* parent)
    : QScrollArea(parent)
{

}

void ScrollArea::forceResize()
{
    resizeEvent(new QResizeEvent(size(), size()));
}
