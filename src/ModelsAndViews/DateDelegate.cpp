#include "DateDelegate.h"

#include <qwtble/QwtBleUtilities.h>

DateDelegate::DateDelegate(QObject* parent)
    : QStyledItemDelegate(parent),
      defaultDateFormat_(qwt_ble_utilities::getDefaultDateFormat())
{
}

QString DateDelegate::displayText(const QVariant& value,
                                  [[maybe_unused]] const QLocale& locale) const
{
    return value.toDate().toString(defaultDateFormat_);
}
