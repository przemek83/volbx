#ifndef PICKER_H
#define PICKER_H

#include <qwt_plot_picker.h>

/**
 * @brief Base class for pickers - display of coordinates on mouse action.
 */
class Picker : public QwtPlotPicker
{
public:
    explicit Picker(QWidget *parent);

    virtual ~Picker() = default;

    virtual int getAreaOfMouse();

    bool getMouseInWidget();

protected:
    virtual QwtText trackerTextF(const QPointF &pos)const = 0;

    virtual void widgetEnterEvent(QEvent* event);

    virtual void widgetLeaveEvent(QEvent* event);

private:
    Q_DISABLE_COPY(Picker)

    bool mouseInWidget_;
};

#endif // PICKER_H
