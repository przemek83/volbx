#pragma once

#include <QStyledItemDelegate>

/**
 * @brief Delegate for date type.
 */
class DateDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit DateDelegate(QObject* parent = nullptr);

    QString displayText(const QVariant& value,
                        const QLocale& locale) const override;

private:
    QString defaultDateFormat_;
};
