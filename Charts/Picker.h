#ifndef PICKER_H
#define PICKER_H

#include <qwt_plot_picker.h>

/**
 * @brief Base class for pickers - display of coordinates on mouse action.
 */
class Picker : public QwtPlotPicker
{
public:
    explicit Picker(QWidget* parent);

    ~Picker() override = default;

    virtual int getAreaOfMouse();

    bool getMouseInWidget();

protected:
    QwtText trackerTextF(const QPointF& pos) const override = 0;

    void widgetEnterEvent(QEvent* event) override;

    void widgetLeaveEvent(QEvent* event) override;

private:
    Q_DISABLE_COPY(Picker)

    bool mouseInWidget_;
};

#endif // PICKER_H
