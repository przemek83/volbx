#ifndef HISTPICKER_H
#define HISTPICKER_H

#include "Picker.h"

class HistPicker : public Picker
{
    Q_OBJECT
public:
    explicit HistPicker(QWidget* parent);

    virtual ~HistPicker() override = default;

    HistPicker& operator=(const HistPicker& other) = delete;
    HistPicker(const HistPicker& other) = delete;

    HistPicker& operator=(HistPicker&& other) = delete;
    HistPicker(HistPicker&& other) = delete;

protected:
    QwtText trackerTextF(const QPointF& pos) const override;
};

#endif // HISTPICKER_H
