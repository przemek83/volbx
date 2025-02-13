#pragma once

#include <QStyledItemDelegate>

class NumericDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit NumericDelegate(QObject* parent);

    QString displayText(const QVariant& value,
                        const QLocale& locale) const override;

    const int decimalPlaces_{2};
};
