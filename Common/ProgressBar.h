#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <QWidget>
#include <QPen>
#include <QApplication>
#include <cmath>

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

    ProgressBar(ProgressBar::ProgressTitle title, int max, QWidget* parent = nullptr);

    ~ProgressBar() override = default;

    ProgressBar& operator=(const ProgressBar& other) = delete;
    ProgressBar(const ProgressBar& other) = delete;

    ProgressBar& operator=(ProgressBar&& other) = delete;
    ProgressBar(ProgressBar&& other) = delete;

    inline void updateProgress(int newValue)
    {
        int newPercent = lround(newValue * 1.0 / maxValue_ * 100);
        if (newPercent > currentPercent_)
        {
            currentPercent_ = newPercent;
            update();
            QApplication::processEvents();
        }
    }

protected:
    void paintEvent(QPaintEvent* event) override;

    void timerEvent(QTimerEvent* event) override;

private:
    int currentPercent_;

    int maxValue_;

    QString title_;

    int lineWeidth_;

    ///Font used to display %.
    QFont counterFont_;

    ///Font used do draw title.
    QFont titleFont_;

    ///Area of round display.
    QRect arcRectangle_;

    ///Title area.
    QRect titleRectangle_;

    QPen pen_;

    QBrush brush_;

    QVector<QString> initNames(char newLine);
};

#endif // PROGRESSBAR_H
