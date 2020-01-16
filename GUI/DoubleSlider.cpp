#include "DoubleSlider.h"

#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOptionSlider>
#include <qdrawutil.h>

#include "Common/Constants.h"

DoubleSlider::DoubleSlider(int min, int max, QWidget* parent) :
    QSlider(parent), minValue_(min), maxValue_(max)
{
    setOrientation(Qt::Horizontal);

    setCurrentMin(minValue_);
    setCurrentMax(maxValue_);

    lastEmittedMin_ = getCurrentMin();
    lastEmittedMax_ = getCurrentMax();

    this->setMouseTracking(true);

    //Refresh.
    refreshTimer_.setSingleShot(false);
    QObject::connect(&refreshTimer_, SIGNAL(timeout()), this, SLOT(update()));
    refreshTimer_.start(REFRESH_TIMER_INTERVAL);
}

QSize DoubleSlider::sizeHint() const
{
    return QSize(SIZE_HINT_WIDTH, SIZE_HINT_HEIGH);
}

int DoubleSlider::getCurrentMin()
{
    return qRound(currentMin_ + minValue_);
}

int DoubleSlider::getCurrentMax()
{
    return qRound(currentMax_ + minValue_);
}

int DoubleSlider::getMinimum()
{
    return static_cast<int>(maxValue_);
}

int DoubleSlider::getMaximum()
{
    return static_cast<int>(maxValue_);
}

void DoubleSlider::setCurrentMin(double currentMinToSet)
{
    double newCurrentMin = currentMinToSet;

    if (newCurrentMin >= minValue_ && currentMin_ != newCurrentMin)
    {
        currentMin_ = newCurrentMin;
        if (lastEmittedMin_ != static_cast<int>(currentMinToSet))
        {
            lastEmittedMin_ = currentMinToSet;
            Q_EMIT minChanged(currentMinToSet);
        }
    }

    if (newCurrentMin > currentMax_)
    {
        setCurrentMax(currentMinToSet);
    }

    this->update();
}

void DoubleSlider::setCurrentMax(double currentMaxToSet)
{
    double newCurrentMax = currentMaxToSet;

    if (newCurrentMax  <= maxValue_ && currentMax_ != newCurrentMax)
    {
        currentMax_ = newCurrentMax;
        if (lastEmittedMax_ != static_cast<int>(currentMaxToSet))
        {
            lastEmittedMax_ = currentMaxToSet;
            Q_EMIT maxChanged(currentMaxToSet);
        }
    }

    if (newCurrentMax  < currentMin_)
    {
        setCurrentMin(currentMaxToSet);
    }

    this->update();
}

void DoubleSlider::setValueMin(int minimumToSet)
{
    if (minimumToSet < maxValue_)
    {
        minValue_ = minimumToSet;
    }

    if (currentMin_ < minValue_)
    {
        currentMin_ = minValue_;
    }

    this->update();
}

void DoubleSlider::setValueMax(int maximumToSet)
{
    if (maximumToSet > minValue_)
    {
        maxValue_ = maximumToSet;
    }

    if (currentMax_ > maxValue_)
    {
        currentMax_ = maxValue_;
    }

    this->update();
}

void DoubleSlider::mousePressEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        mousePositionX_ =
            static_cast<float>(event->x()) / static_cast<float>(this->width()) * (100.0 + cursorSize_);
    }

    moving_ = 0;
}

void DoubleSlider::mouseReleaseEvent(QMouseEvent* event)
{
    moving_ = 0;
    this->mouseMoveEvent(event);
}

void DoubleSlider::mouseMoveEvent(QMouseEvent* event)
{
    float mouseX =
        static_cast<float>(event->x()) / static_cast<float>(this->width()) * (100.0 + cursorSize_);
    float minX = ((currentMin_ - minValue_) / (maxValue_ - minValue_) * MAX_PERCENT);
    float maxX = ((currentMax_ - minValue_) / (maxValue_ - minValue_) * MAX_PERCENT);

    isOnMinHandle_ = ((((mouseX > minX) && (mouseX < minX + cursorSize_)) ||
                       ((mousePositionX_ > minX) && (mousePositionX_ < minX + cursorSize_)))
                      && (moving_ != 2)
                      && !isOnMaxHandle_);

    isOnMaxHandle_ = ((((mouseX > maxX) && (mouseX < maxX + cursorSize_)) ||
                       ((mousePositionX_ > maxX) && (mousePositionX_ < maxX + cursorSize_)))
                      && (moving_ != 1)
                      && !isOnMinHandle_);

    if (event->buttons() & Qt::LeftButton)
    {
        if ((moving_ != 2) && isOnMinHandle_)
        {
            minX += mouseX - mousePositionX_;
            if (minX < 0)
            {
                minX = 0;
            }
            if (minX > MAX_PERCENT)
            {
                minX = MAX_PERCENT;
            }
            setCurrentMin(minX / MAX_PERCENT * (maxValue_ - minValue_) + minValue_);

            if (currentMax_ < currentMin_)
            {
                setCurrentMax(minX);
            }

            moving_ = 1;
        }

        if ((moving_ != 1) && isOnMaxHandle_)
        {
            maxX += mouseX - mousePositionX_;
            if (maxX < 0)
            {
                maxX = 0;
            }
            if (maxX > MAX_PERCENT)
            {
                maxX = MAX_PERCENT;
            }
            setCurrentMax(maxX / MAX_PERCENT * (maxValue_ - minValue_) + minValue_);
            if (currentMin_ > currentMax_)
            {
                setCurrentMin(maxX);
            }

            moving_ = 2;
        }
    }
    else
    {
        moving_ = 0;
    }

    mousePositionX_ = mouseX;
}

void DoubleSlider::paintEvent(QPaintEvent* event)
{
    if (minValue_ == maxValue_)
    {
        return;
    }

    double minX = ((currentMin_ - minValue_) / (maxValue_ - minValue_) * MAX_PERCENT);
    double maxX = ((currentMax_ - minValue_) / (maxValue_ - minValue_) * MAX_PERCENT);

    QPainter p(this);

    //Default style.
    QStyleOptionSlider defaultStyle;
    initStyleOption(&defaultStyle);

    QStyleOptionSlider opt = defaultStyle;

    //Slider bar.
    opt.subControls = QStyle::SC_SliderGroove;
    style()->drawComplexControl(QStyle::CC_Slider, &opt, &p, this);

    //Slider bar between handles.
    QPainter painter(this);
    painter.setClipRegion(event->region());
    QRect innerRect(defaultStyle.rect);
    int coursorHalf = cursorSize_ / 2;

    double fromX =
        innerRect.width() / (maxValue_ - minValue_) * (currentMin_ - minValue_);
    double width =
        innerRect.width() / (maxValue_ - minValue_) * (currentMax_ - minValue_) - fromX;

    QRect rSlot = QRect(innerRect.x() + fromX,
                        innerRect.y() + (innerRect.height() - coursorHalf) / 2,
                        innerRect.x() + width,
                        coursorHalf);

    QBrush brush = palette().brush(QPalette::Dark);
    qDrawShadePanel(&painter, rSlot, palette(), true, 1, &brush);

    //Left handle.
    QStyleOptionSlider opt2 = defaultStyle;
    if (minX == MAX_PERCENT)
    {
        opt2.sliderPosition = minX - 1;
    }
    else
    {
        opt2.sliderPosition = minX;
    }

    if (moving_ == 1)
    {
        opt2.state = QStyle::State_Raised;
    }

    opt2.subControls = QStyle::SC_SliderHandle;
    style()->drawComplexControl(QStyle::CC_Slider, &opt2, &p, this);

    //Righ handle.
    QStyleOptionSlider opt3 = defaultStyle;
    if (maxX == MAX_PERCENT)
    {
        opt3.sliderPosition = maxX - 1;
    }
    else
    {
        opt3.sliderPosition = maxX;
    }

    if (moving_ == 2)
    {
        opt3.state = QStyle::State_Raised;
    }

    opt3.subControls = QStyle::SC_SliderHandle;
    style()->drawComplexControl(QStyle::CC_Slider, &opt3, &p, this);
}
