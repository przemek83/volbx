#pragma once

#include <QStyledItemDelegate>

class DateDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit DateDelegate(QObject* parent);

    QString displayText(const QVariant& value,
                        const QLocale& locale) const override;

private:
    QString defaultDateFormat_;
};
