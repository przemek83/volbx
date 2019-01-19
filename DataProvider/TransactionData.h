#ifndef TRANSACTIONDATA_H
#define TRANSACTIONDATA_H

#include <QDate>
#include <QVariant>

/**
 * @brief Object of those class are used for computation. 3 related values needed.
 */
class TransactionData
{
public:
    TransactionData();

    virtual ~TransactionData();

    float pricePerMeter_;

    QDate date_;

    QVariant groupedBy_;

    void clear();

    void print() const;
};

#endif // TRANSACTIONDATA_H
