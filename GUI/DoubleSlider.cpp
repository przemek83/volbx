#include "DoubleSlider.h"

#include <cmath>

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

    // Cannot use new connect syntax as timeout has param QPrivateSignal.
    connect(&refreshTimer_, SIGNAL(timeout()), this, SLOT(update()));
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

    if (newCurrentMin >= minValue_ && !Constants::doublesAreEqual(currentMin_, newCurrentMin))
    {
        currentMin_ = newCurrentMin;
        if (lastEmittedMin_ != static_cast<int>(currentMinToSet))
        {
            lastEmittedMin_ = static_cast<int>(currentMinToSet);
            Q_EMIT minChanged(static_cast<int>(currentMinToSet));
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

    if (newCurrentMax  <= maxValue_ && !Constants::doublesAreEqual(currentMax_, newCurrentMax))
    {
        currentMax_ = newCurrentMax;
        if (lastEmittedMax_ != static_cast<int>(currentMaxToSet))
        {
            lastEmittedMax_ = static_cast<int>(currentMaxToSet);
            Q_EMIT maxChanged(static_cast<int>(currentMaxToSet));
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
    if ((event->buttons() & Qt::LeftButton) != 0U)
    {
        mousePositionX_ =
            static_cast<double>(event->x()) / static_cast<double>(this->width()) * (100.0 + cursorSize_);
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
    double mouseX =
        static_cast<double>(event->x()) / static_cast<double>(this->width()) * (100.0 + cursorSize_);
    double minX = ((currentMin_ - minValue_) / (maxValue_ - minValue_) * MAX_PERCENT);
    double maxX = ((currentMax_ - minValue_) / (maxValue_ - minValue_) * MAX_PERCENT);

    isOnMinHandle_ = ((((mouseX > minX) && (mouseX < minX + cursorSize_)) ||
                       ((mousePositionX_ > minX) && (mousePositionX_ < minX + cursorSize_)))
                      && (moving_ != 2)
                      && !isOnMaxHandle_);

    isOnMaxHandle_ = ((((mouseX > maxX) && (mouseX < maxX + cursorSize_)) ||
                       ((mousePositionX_ > maxX) && (mousePositionX_ < maxX + cursorSize_)))
                      && (moving_ != 1)
                      && !isOnMinHandle_);

    if ((event->buttons() & Qt::LeftButton) != 0U)
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

int DoubleSlider::getLeftHandlePosition() const
{
    const double range {maxValue_ - minValue_};
    const int minX =
        static_cast<int>((currentMin_ - minValue_) / range * MAX_PERCENT);
    return (minX == MAX_PERCENT ? minX - 1 : minX);
}

int DoubleSlider::getRightHandlePosition() const
{
    const double range {maxValue_ - minValue_};
    const int maxX =
        static_cast<int>((currentMax_ - minValue_) / range * MAX_PERCENT);
    return (maxX == MAX_PERCENT ? maxX - 1 : maxX);
}

void DoubleSlider::drawSliderBar(QPainter& painter) const
{
    QStyleOptionSlider barStyle;
    initStyleOption(&barStyle);
    QStyleOptionSlider opt = barStyle;
    opt.subControls = QStyle::SC_SliderGroove;

    style()->drawComplexControl(QStyle::CC_Slider, &opt, &painter, this);
}

void DoubleSlider::drawSliderBarBetweenHandles(QPainter& painter) const
{
    QStyleOptionSlider defaultStyle;
    initStyleOption(&defaultStyle);
    const QRect innerRect(defaultStyle.rect);

    const double stepWidth =
        static_cast<double>(innerRect.width()) / MAX_PERCENT;
    const int leftHandlePosition = getLeftHandlePosition();
    const int rightHandlePosition = getRightHandlePosition();
    const int barLeftEnd =
        static_cast<int>(leftHandlePosition * stepWidth + stepWidth / 2);
    const int barWidth =
        static_cast<int>((rightHandlePosition - leftHandlePosition) * stepWidth + stepWidth / 2);
    const int coursorHalf = cursorSize_ / 2;
    const QRect rSlot = QRect(innerRect.x() + barLeftEnd,
                              innerRect.y() + (innerRect.height() - coursorHalf) / 2,
                              innerRect.x() + barWidth,
                              coursorHalf);

    const QBrush brush = palette().brush(QPalette::Dark);
    qDrawShadePanel(&painter, rSlot, palette(), true, 1, &brush);
}

QStyleOptionSlider DoubleSlider::getHandleStyle(bool moving, int position) const
{
    QStyleOptionSlider handleStyle;
    initStyleOption(&handleStyle);
    handleStyle.sliderPosition = position;
    if (moving)
    {
        handleStyle.state = QStyle::State_Raised;
    }
    handleStyle.subControls = QStyle::SC_SliderHandle;
    return handleStyle;
}

void DoubleSlider::drawHandles(QPainter& painter) const
{
    const QStyleOptionSlider leftStyle =
        getHandleStyle(moving_ == 1, getLeftHandlePosition());
    style()->drawComplexControl(QStyle::CC_Slider, &leftStyle, &painter, this);

    const QStyleOptionSlider rightStyle =
        getHandleStyle(moving_ == 2, getRightHandlePosition());
    style()->drawComplexControl(QStyle::CC_Slider, &rightStyle, &painter, this);
}

void DoubleSlider::paintEvent(QPaintEvent* event)
{
    if (Constants::doublesAreEqual(minValue_, maxValue_))
    {
        return;
    }

    QPainter painter(this);
    painter.setClipRegion(event->region());

    drawSliderBar(painter);
    drawSliderBarBetweenHandles(painter);
    drawHandles(painter);
}
