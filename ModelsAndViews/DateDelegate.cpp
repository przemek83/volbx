#include "DateDelegate.h"

#include <QwtBleUtilities.h>
#include <QDate>

DateDelegate::DateDelegate(QObject* parent)
    : QStyledItemDelegate(parent),
      defaultDateFormat_(QwtBleUtilities::getDefaultDateFormat())
{
}

QString DateDelegate::displayText(const QVariant& value,
                                  const QLocale& /*locale*/) const
{
    return value.toDate().toString(defaultDateFormat_);
}
