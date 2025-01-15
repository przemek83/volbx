#include "DateDelegate.h"

#include <qwtble/QwtBleUtilities.h>

DateDelegate::DateDelegate(QObject* parent) : QStyledItemDelegate{parent} {}

QString DateDelegate::displayText(const QVariant& value,
                                  [[maybe_unused]] const QLocale& locale) const
{
    return value.toDate().toString(defaultDateFormat_);
}
