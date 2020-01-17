#ifndef BOXPICKER_H
#define BOXPICKER_H

#include "Picker.h"

class BoxPicker : public Picker
{
    Q_OBJECT
public:
    explicit BoxPicker(QWidget* parent);

    virtual ~BoxPicker() override = default;

    BoxPicker& operator=(const BoxPicker& other) = delete;
    BoxPicker(const BoxPicker& other) = delete;

    BoxPicker& operator=(BoxPicker&& other) = delete;
    BoxPicker(BoxPicker&& other) = delete;

protected:
    QwtText trackerTextF(const QPointF& pos) const override;
};
#endif // BOXPICKER_H
