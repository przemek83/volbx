#include "NumericDelegate.h"

#include <QDebug>
#include <QwtBleUtilities.h>

NumericDelegate::NumericDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{

}

QString NumericDelegate::displayText(const QVariant& value,
                                     const QLocale& /*locale*/) const
{
    return QwtBleUtilities::floatToStringUsingLocale(value.toFloat(), 2);
}
