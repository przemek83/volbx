#include "DatasetDummy.h"

DatasetDummy::DatasetDummy(const QString& name, QObject* parent)
    : Dataset(name, parent)
{
}

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
