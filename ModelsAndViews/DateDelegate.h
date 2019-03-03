#ifndef DATEDELEGATE_H
#define DATEDELEGATE_H

#include <QStyledItemDelegate>

/**
 * @brief Delegate for date type.
 */
class DateDelegate : public QStyledItemDelegate
{
public:
    explicit DateDelegate(QObject* parent = nullptr);

    ~DateDelegate() override = default;

    QString displayText(const QVariant &value, const QLocale &locale) const override;

private:
    Q_DISABLE_COPY(DateDelegate)

    QString defaultDateFormat_;
};

#endif // DATEDELEGATE_H
