#include "DateDelegate.h"

#include <qwtble/QwtBleUtilities.h>

DateDelegate::DateDelegate(QObject* parent)
    : QStyledItemDelegate(parent),
      defaultDateFormat_(QwtBleUtilities::getDefaultDateFormat())
{
}

QString DateDelegate::displayText(const QVariant& value,
                                  [[maybe_unused]] const QLocale& locale) const
{
    return value.toDate().toString(defaultDateFormat_);
}
