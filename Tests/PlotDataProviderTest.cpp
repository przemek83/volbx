#include "PlotDataProviderTest.h"

#include <QtTest/QtTest>

#include <PlotDataProvider.h>

PlotDataProviderTest::PlotDataProviderTest(QObject* parent) : QObject(parent) {}

void PlotDataProviderTest::testNotSetGroupingColumn()
{
    PlotDataProvider provider;
    QCOMPARE(provider.getGroupByColumn(), Constants::NOT_SET_COLUMN);
}
