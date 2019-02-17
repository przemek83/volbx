#include <QTimer>
#include <QTime>
#include <QPainter>
#include <QDebug>
#include <QApplication>

#include "Common/Constants.h"

#include "ProgressBar.h"

ProgressBar::ProgressBar(ProgressBar::ProgressTitle title,
                         int max,
                         QWidget* parent)
    : QWidget(parent),
      currentPercent_(0),
      maxValue_(max),
      lineWeidth_(10)
{
    static const char newLine('\n');
    static QVector<QString> progressTitles(static_cast<int>(PROGRESS_TITLE_END));

    initNames(progressTitles, newLine);

    title_ = progressTitles[title];
    setWindowTitle(QString(title_).replace(newLine, ' '));

    if( nullptr == parent )
    {
        setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
        setWindowModality(Qt::ApplicationModal);
    }

    //Counter without %.
    if( 0 == maxValue_ )
    {
        startTimer(50);
    }

    QSize size(120,140);
    resize(size);

    QColor color(127, 0, 127);
    brush_ = QBrush(color);

    pen_.setColor(color);
    pen_.setWidth(lineWeidth_);
    pen_.setStyle(Qt::SolidLine);

    int fontPointSizen = QApplication::font().pointSize();
    counterFont_.setPointSize(fontPointSizen*2.5);
    counterFont_.setStyleStrategy(QFont::PreferAntialias);
    titleFont_.setPointSize(fontPointSizen*1.5);
    titleFont_.setBold(true);

    QSize arcSize(width() - 4 * lineWeidth_, width() - 4 * lineWeidth_);
    arcRectangle_ = QRect(width()/2-arcSize.width()/2,
                         (width()-2*lineWeidth_)/2-arcSize.height()/2,
                         arcSize.width(),
                         arcSize.height());

    titleRectangle_ = QRect(0,
                          height()-lineWeidth_*4,
                          width(),
                          lineWeidth_*4);

    QWidget* activeWidget = QApplication::activeWindow();
    if(nullptr != activeWidget)
    {
        move(QApplication::activeWindow()->geometry().center() -
             geometry().center());
    }
    show();
}

void ProgressBar::initNames(QVector<QString>& titles, const char newLine)
{
    static bool initialized = false;

    if ( false == initialized )
    {
        titles[PROGRESS_TITLE_LOADING] =
            QObject::tr("Loading") + newLine + QObject::tr("data");
        titles[PROGRESS_TITLE_SAVING] =
            QObject::tr("Saving") + newLine + QObject::tr("data");
        titles[PROGRESS_TITLE_RECOMPUTING] =
            QObject::tr("Calculating") + newLine + QObject::tr("data");
        titles[PROGRESS_TITLE_RECOMPUTING_TREND] =
            QObject::tr("Calculating") + newLine + QObject::tr("trend");
        titles[PROGRESS_TITLE_VIEW_CREATION] =
            QObject::tr("Creating") + newLine + QObject::tr("view");
        titles[PROGRESS_TITLE_DETECTING_COLUMN_TYPES] =
            QObject::tr("File") + newLine + QObject::tr("analysis");

        initialized = true;
    }
}

void ProgressBar::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(pen_);
    painter.setBrush(brush_);
    painter.setFont(counterFont_);

    int startAngle;
    int spanAngle;

    //Counter without %.
    if(0 == maxValue_)
    {
        int step = 45;
        startAngle = currentPercent_*3.6 * 16;
        spanAngle = -step * 16;
        painter.drawArc(arcRectangle_, startAngle, spanAngle);
        startAngle = (180+currentPercent_*3.6) * 16;
        painter.drawArc(arcRectangle_, startAngle, spanAngle);
    }
    else
    {
        startAngle = 90 * 16;
        spanAngle = -currentPercent_*3.6 * 16;
        painter.drawArc(arcRectangle_, startAngle, spanAngle);
        painter.drawText(arcRectangle_,
                         Qt::AlignCenter,
                         QString::number(currentPercent_, 'f', 0) + "%");

    }

    painter.setFont(titleFont_);

    painter.drawText(titleRectangle_, Qt::AlignCenter, title_ + "...");

    if(0 != maxValue_)
    {
        setWindowTitle(QString::number(currentPercent_) + "% " +
                       QString(title_).replace('\n', ' ') + "...");

    }
}

 void ProgressBar::timerEvent(QTimerEvent* /*event*/)
 {
     currentPercent_++;
     currentPercent_ %= 100;
     update();
 }
