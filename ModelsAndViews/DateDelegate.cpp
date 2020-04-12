#include "DateDelegate.h"

#include <QwtBleUtilities.h>
#include <QDate>
#include <QDebug>

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
