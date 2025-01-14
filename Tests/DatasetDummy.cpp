#include "DatasetDummy.h"

bool DatasetDummy::analyze() { return true; }

std::tuple<bool, QVector<QVector<QVariant> > > DatasetDummy::getSample()
{
    return {true, {}};
}

std::tuple<bool, QVector<QVector<QVariant> > > DatasetDummy::getAllData()
{
    return {true, {}};
}

void DatasetDummy::closeZip() {}
