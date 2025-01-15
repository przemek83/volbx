#pragma once

#include <qwtble/QwtBleUtilities.h>
#include <QStyledItemDelegate>

class DateDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit DateDelegate(QObject* parent);

    QString displayText(const QVariant& value,
                        const QLocale& locale) const override;

private:
    QString defaultDateFormat_{qwt_ble_utilities::getDefaultDateFormat()};
};
