#ifndef DATEDELEGATE_H
#define DATEDELEGATE_H

#include <QStyledItemDelegate>

/**
 * @brief Delegate for date type.
 */
class DateDelegate : public QStyledItemDelegate
{
public:
    explicit DateDelegate(QObject* parent = 0);

    virtual ~DateDelegate() = default;

    virtual QString displayText(const QVariant& value, const QLocale& locale) const;

private:
    Q_DISABLE_COPY(DateDelegate)

    QString defaultDateFormat_;
};

#endif // DATEDELEGATE_H
