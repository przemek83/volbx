#include "NumericDelegate.h"

#include <QwtBleUtilities.h>

NumericDelegate::NumericDelegate(QObject* parent) : QStyledItemDelegate(parent)
{
}

QString NumericDelegate::displayText(
    const QVariant& value, [[maybe_unused]] const QLocale& locale) const
{
    return QwtBleUtilities::doubleToStringUsingLocale(value.toDouble(), 2);
}
