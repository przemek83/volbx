#pragma once

#include <QDate>
#include <QMetaType>
#include <QVariant>

/**
 * @brief Struct used for computation. 3 related values needed.
 */
struct TransactionData
{
public:
    QDate date_;

    QVariant groupedBy_;

    double pricePerMeter_{0.};
};

Q_DECLARE_METATYPE(TransactionData)
