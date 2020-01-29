#ifndef DOUBLESLIDER_H
#define DOUBLESLIDER_H

#include <QSlider>
#include <QTimer>

/**
 * @brief Slider with 2 handles.
 */
class DoubleSlider : public QSlider
{
    Q_OBJECT
public:
    DoubleSlider(int min, int max, QWidget* parent = nullptr);

    ~DoubleSlider() override = default;

    DoubleSlider& operator=(const DoubleSlider& other) = delete;
    DoubleSlider(const DoubleSlider& other) = delete;

    DoubleSlider& operator=(DoubleSlider&& other) = delete;
    DoubleSlider(DoubleSlider&& other) = delete;

    int getCurrentMin();

    int getCurrentMax();

    int getMinimum();

    int getMaximum();

    void setValueMin(int minimumToSet);

    void setValueMax(int maximumToSet);

    QSize sizeHint() const override;

public Q_SLOTS:
    void setCurrentMin(double currentMinToSet);

    void setCurrentMax(double currentMaxToSet);

protected:
    void paintEvent(QPaintEvent* event) override;

    void mousePressEvent(QMouseEvent* event) override;

    void mouseReleaseEvent(QMouseEvent* event) override;

    void mouseMoveEvent(QMouseEvent* event) override;

private:
    double currentMin_ {0.};

    double currentMax_ {0.};

    double minValue_;

    double maxValue_;

    static constexpr int cursorSize_{16};

    double mousePositionX_{0};

    ///Timer for refreshing.
    QTimer refreshTimer_;

    int lastEmittedMin_ {0};

    int lastEmittedMax_ {0};

    ///Flag to remember handle is moving.
    int moving_{0};

    ///Mouse on left handle.
    bool isOnMinHandle_{false};

    ///Mouse on right handle.
    bool isOnMaxHandle_{false};

    static constexpr int MAX_PERCENT{100};

    static constexpr int REFRESH_TIMER_INTERVAL{25};

    static constexpr int SIZE_HINT_WIDTH{120};

    static constexpr int SIZE_HINT_HEIGH{40};

Q_SIGNALS:
    void minChanged(int);

    void maxChanged(int);
};

#endif // DOUBLESLIDER_H
