#include "DateDelegate.h"

#include <QDate>
#include <QDebug>

#include "Common/Constants.h"

DateDelegate::DateDelegate(QObject* parent)
    : QStyledItemDelegate(parent), defaultDateFormat_(Constants::getDefaultDateFormat())
{
}

QString DateDelegate::displayText(const QVariant& value,
                                  const QLocale& /*locale*/) const
{
    return value.toDate().toString(defaultDateFormat_);
}
