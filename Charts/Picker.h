#ifndef PICKER_H
#define PICKER_H

#include <qwt_plot_picker.h>

/**
 * @brief Base class for pickers - display of coordinates on mouse action.
 */
class Picker : public QwtPlotPicker
{
    Q_OBJECT
public:
    explicit Picker(QWidget* parent);

    virtual ~Picker() override = default;

    Picker& operator=(const Picker& other) = delete;
    Picker(const Picker& other) = delete;

    Picker& operator=(Picker&& other) = delete;
    Picker(Picker&& other) = delete;

    virtual int getAreaOfMouse();

    bool getMouseInWidget();

protected:
    QwtText trackerTextF(const QPointF& pos) const override = 0;

    void widgetEnterEvent(QEvent* event) override;

    void widgetLeaveEvent(QEvent* event) override;

private:
    bool mouseInWidget_;
};

#endif // PICKER_H
