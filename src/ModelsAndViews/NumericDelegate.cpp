#include "NumericDelegate.h"

#include <qwtble/QwtBleUtilities.h>

NumericDelegate::NumericDelegate(QObject* parent) : QStyledItemDelegate(parent)
{
}

QString NumericDelegate::displayText(
    const QVariant& value, [[maybe_unused]] const QLocale& locale) const
{
    return qwt_ble_utilities::doubleToStringUsingLocale(value.toDouble(),
                                                        decimalPlaces_);
}
