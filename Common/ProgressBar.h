#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <QWidget>
#include <QPen>
#include <QApplication>

/**
 * @brief Round progress bar to show progress. If max != 0 '%' are used.
 */
class ProgressBar : public QWidget
{
    Q_OBJECT
public:
    enum ProgressTitle
    {
        PROGRESS_TITLE_LOADING = 0,
        PROGRESS_TITLE_SAVING,
        PROGRESS_TITLE_RECOMPUTING,
        PROGRESS_TITLE_RECOMPUTING_TREND,
        PROGRESS_TITLE_VIEW_CREATION,
        PROGRESS_TITLE_DETECTING_COLUMN_TYPES,
        PROGRESS_TITLE_END
    };

    ProgressBar(ProgressBar::ProgressTitle title, int max, QWidget* parent = 0);

    virtual ~ProgressBar() = default;

    inline void updateProgress(int newValue)
    {
        int newPercent = newValue * 1.0 / maxValue_ * 100;
        if( newPercent > currentPercent_ )
        {
            currentPercent_ = newPercent;
            update();
            QApplication::processEvents();
        }
    }

protected:
    void paintEvent(QPaintEvent *event);

    void timerEvent(QTimerEvent *event);

private:
    Q_DISABLE_COPY(ProgressBar)

    int currentPercent_;

    int maxValue_;

    QString title_;

    int lineWeidth_;

    QPen pen_;

    QBrush brush_;

    ///Font used to display %.
    QFont counterFont_;

    ///Font used do draw title.
    QFont titleFont_;

    ///Area of round display.
    QRect arcRectangle_;

    ///Title area.
    QRect titleRectangle_;

    void initNames(QVector<QString>& titles, const char newLine);
};

#endif // PROGRESSBAR_H
