#ifndef LINEARPICKER_H
#define LINEARPICKER_H

#include "Picker.h"

class LinearPicker : public Picker
{
    Q_OBJECT
public:
    explicit LinearPicker(QWidget* parent);

    virtual ~LinearPicker() override = default;

    LinearPicker& operator=(const LinearPicker& other) = delete;
    LinearPicker(const LinearPicker& other) = delete;

    LinearPicker& operator=(LinearPicker&& other) = delete;
    LinearPicker(LinearPicker&& other) = delete;

protected:
    [[nodiscard]] QwtText trackerTextF(const QPointF& pos) const override;
};

#endif // LINEARPICKER_H
