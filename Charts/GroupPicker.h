#ifndef GROUPPICKER_H
#define GROUPPICKER_H

#include "Picker.h"

class GroupPicker : public Picker
{
    Q_OBJECT
public:
    explicit GroupPicker(QWidget* parent);

    virtual ~GroupPicker() override = default;

    GroupPicker& operator=(const GroupPicker& other) = delete;
    GroupPicker(const GroupPicker& other) = delete;

    GroupPicker& operator=(GroupPicker&& other) = delete;
    GroupPicker(GroupPicker&& other) = delete;

protected:
    QwtText trackerTextF(const QPointF& pos) const override;
};

#endif // GROUPPICKER_H
