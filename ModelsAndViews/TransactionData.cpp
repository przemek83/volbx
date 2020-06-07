#include "TransactionData.h"

void TransactionData::clear()
{
    pricePerMeter_ = 0;
    date_ = QDate();
    groupedBy_ = QVariant();
}
