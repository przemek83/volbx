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
    DoubleSlider(int min, int max, QWidget *parent = 0);

    /**
     * Destruktor.
     */
    virtual ~DoubleSlider();

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
    QSize sizeHint() const;

public slots:
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
    void paintEvent(QPaintEvent *event);

    /**
     * Overriden mouse press method.
     * @param event mouse event.
     */
    void mousePressEvent(QMouseEvent *event);

    /**
     * Overriden mouse release method.
     * @param event mouse event.
     */
    void mouseReleaseEvent(QMouseEvent *event);

    /**
     * Overriden mouse move method.
     * @param event mouse event.
     */
    void mouseMoveEvent(QMouseEvent *event);

private:
    Q_DISABLE_COPY(DoubleSlider)

    ///Actual minimum.
    double currentMin_;

    ///Actual maximum.
    double currentMax_;

    ///Minimum.
    double minValue_;

    ///Maximum.
    double maxValue_;

    ///Coursor size.
    double cursorSize_;

    ///Mouse position on axis x.
    double mousePositionX_;

    ///Flag to remember handle is moving.
    int moving_;

    ///Mouse on left handle.
    bool isOnMinHandle_;

    ///Mouse on right handle.
    bool isOnMaxHandle_;

    ///Timer for refreshing.
    QTimer refreshTimer_;

    ///Last emitted minimum.
    int lastEmittedMin_;

    ///Last emitted maximum.
    int lastEmittedMax_;

signals:
    ///Emitted after actual minimum changed.
    void minChanged(int);

    ///Emitted after actual maximum changed.
    void maxChanged(int);
};

#endif // DOUBLESLIDER_H
