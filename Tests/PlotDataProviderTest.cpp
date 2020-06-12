#include "PlotDataProviderTest.h"

#include <QtTest/QtTest>

#include <PlotDataProvider.h>

PlotDataProviderTest::PlotDataProviderTest(QObject* parent) : QObject(parent) {}

void PlotDataProviderTest::testNotSetGroupingColumn()
{
    PlotDataProvider provider;
    QCOMPARE(provider.getGroupByColumn(), Constants::NOT_SET_COLUMN);
}

void PlotDataProviderTest::testRecomputeGroupingDataNotSetColumn()
{
    PlotDataProvider provider;
    QSignalSpy spy(&provider, &PlotDataProvider::groupingPlotDataChanged);
    provider.recomputeGroupingData({}, Constants::NOT_SET_COLUMN,
                                   ColumnType::STRING);
    QCOMPARE(spy.count(), NO_SIGNAL);
}
