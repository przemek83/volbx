#include <QDebug>

#include "Common/Constants.h"

#include "NumericDelegate.h"

NumericDelegate::NumericDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{

}

NumericDelegate::~NumericDelegate()
{

}

QString NumericDelegate::displayText(const QVariant& value,
                                     const QLocale& /*locale*/) const
{
    return Constants::floatToStringUsingLocale(value.toFloat(), 2);
}
