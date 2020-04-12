#ifndef NUMERICDELEGATE_H
#define NUMERICDELEGATE_H

#include <QStyledItemDelegate>

/**
 * @brief Delegate for numeric type.
 */
class NumericDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit NumericDelegate(QObject* parent = nullptr);

    ~NumericDelegate() override = default;

    NumericDelegate& operator=(const NumericDelegate& other) = delete;
    NumericDelegate(const NumericDelegate& other) = delete;

    NumericDelegate& operator=(NumericDelegate&& other) = delete;
    NumericDelegate(NumericDelegate&& other) = delete;

    QString displayText(const QVariant& value,
                        const QLocale& locale) const override;
};

#endif  // NUMERICDELEGATE_H
