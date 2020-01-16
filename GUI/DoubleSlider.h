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
    /**
     * Construktor. Sets default values.
     * @param parent parent widget.
     */
    DoubleSlider(int min, int max, QWidget* parent = nullptr);

    /**
     * Destruktor.
     */
    ~DoubleSlider() override = default;

    DoubleSlider& operator=(const DoubleSlider& other) = delete;
    DoubleSlider(const DoubleSlider& other) = delete;

    DoubleSlider& operator=(DoubleSlider&& other) = delete;
    DoubleSlider(DoubleSlider&& other) = delete;

    /**
     * Get actual minimum.
     * @return actual minimum.
     */
    int getCurrentMin();

    /**
     * Get actual maximum.
     * @return actual maximum.
     */
    int getCurrentMax();

    /**
     * Get minimum.
     * @return minimum.
     */
    int getMinimum();

    /**
     * Get minimum.
     * @return minimum.
     */
    int getMaximum();

    /**
     * Set minimum.
     * @param minimumToSet minimum to set.
     */
    void setValueMin(int minimumToSet);

    /**
     * Set maximum.
     * @param maximumToSet maximum to set.
     */
    void setValueMax(int maximumToSet);

    /**
     * Hint for default size of coursor.
     * @return size.
     */
    QSize sizeHint() const override;

public Q_SLOTS:
    /**
     * Set actual minimum.
     * @param currentMinToSet minimum to set.
     */
    void setCurrentMin(double currentMinToSet);

    /**
     * Set actual maximum.
     * @param currentMaxToSet maximum to set.
     */
    void setCurrentMax(double currentMaxToSet);

protected:
    /**
     * Overriden paint method.
     * @param event paint event.
     */
    void paintEvent(QPaintEvent* event) override;

    /**
     * Overriden mouse press method.
     * @param event mouse event.
     */
    void mousePressEvent(QMouseEvent* event) override;

    /**
     * Overriden mouse release method.
     * @param event mouse event.
     */
    void mouseReleaseEvent(QMouseEvent* event) override;

    /**
     * Overriden mouse move method.
     * @param event mouse event.
     */
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    ///Actual minimum.
    double currentMin_;

    ///Actual maximum.
    double currentMax_;

    ///Minimum.
    double minValue_;

    ///Maximum.
    double maxValue_;

    ///Coursor size.
    static constexpr int cursorSize_{16};

    ///Mouse position on axis x.
    double mousePositionX_{0};

    ///Timer for refreshing.
    QTimer refreshTimer_;

    ///Last emitted minimum.
    int lastEmittedMin_;

    ///Last emitted maximum.
    int lastEmittedMax_;

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
    ///Emitted after actual minimum changed.
    void minChanged(int);

    ///Emitted after actual maximum changed.
    void maxChanged(int);
};

#endif // DOUBLESLIDER_H
