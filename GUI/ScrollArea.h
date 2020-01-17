#ifndef SCROLLAREA_H
#define SCROLLAREA_H

#include <QScrollArea>

class ScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    explicit ScrollArea(QWidget* parent = nullptr);

    virtual ~ScrollArea() override = default;

    ScrollArea& operator=(const ScrollArea& other) = delete;
    ScrollArea(const ScrollArea& other) = delete;

    ScrollArea& operator=(ScrollArea&& other) = delete;
    ScrollArea(ScrollArea&& other) = delete;

    void forceResize();
};


#endif // SCROLLAREA_H
