#ifndef NUMERICDELEGATE_H
#define NUMERICDELEGATE_H

#include <QStyledItemDelegate>

/**
 * @brief Delegate for numeric type.
 */
class NumericDelegate : public QStyledItemDelegate
{
public:
    explicit NumericDelegate(QObject* parent = nullptr);

    ~NumericDelegate() override = default;

    QString displayText(const QVariant& value, const QLocale& locale) const override;

private:
    Q_DISABLE_COPY(NumericDelegate)
};

#endif // NUMERICDELEGATE_H
