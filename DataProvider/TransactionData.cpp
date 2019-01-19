#include <QDebug>

#include "Common/Constants.h"

#include "TransactionData.h"

TransactionData::TransactionData()
{

}

TransactionData::~TransactionData()
{

}

void TransactionData::clear()
{
    pricePerMeter_ = 0;
    date_ = QDate();
    groupedBy_ = QVariant();
}

void TransactionData::print() const
{
    qDebug() << "Price per unit " << pricePerMeter_ << " date " <<
                date_ << " grouped " << groupedBy_;
}
