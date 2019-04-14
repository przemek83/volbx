#ifndef DATEDELEGATE_H
#define DATEDELEGATE_H

#include <QStyledItemDelegate>

/**
 * @brief Delegate for date type.
 */
class DateDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit DateDelegate(QObject* parent = nullptr);

    ~DateDelegate() override = default;

    DateDelegate& operator=(const DateDelegate& other) = delete;
    DateDelegate(const DateDelegate& other) = delete;

    DateDelegate& operator=(DateDelegate&& other) = delete;
    DateDelegate(DateDelegate&& other) = delete;

    QString displayText(const QVariant& value, const QLocale& locale) const override;

private:
    QString defaultDateFormat_;
};

#endif // DATEDELEGATE_H
