#include <QDebug>
#include <QDate>

#include "Common/Constants.h"

#include "DateDelegate.h"

DateDelegate::DateDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
    defaultDateFormat_ = QString(Constants::defaultDateFormat_);
}

DateDelegate::~DateDelegate()
{

}

QString DateDelegate::displayText(const QVariant& value,
                                  const QLocale& /*locale*/) const
{
    return value.toDate().toString(defaultDateFormat_);
}
