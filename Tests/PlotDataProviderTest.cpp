#include "PlotDataProviderTest.h"

#include <QtTest/QtTest>

#include <PlotDataProvider.h>

PlotDataProviderTest::PlotDataProviderTest(QObject* parent) : QObject(parent) {}

void PlotDataProviderTest::testRecomputeGroupingDataWrongColumnFormat()
{
    checkRecomputeGroupingDataForColumnType(ColumnType::DATE);
    checkRecomputeGroupingDataForColumnType(ColumnType::NUMBER);
    checkRecomputeGroupingDataForColumnType(ColumnType::UNKNOWN);
}

void PlotDataProviderTest::testRecomputeGroupingData()
{
    PlotDataProvider provider;
    QSignalSpy spy(&provider, &PlotDataProvider::groupingPlotDataChanged);
    provider.recomputeGroupingData(calcData_, ColumnType::STRING);
    QCOMPARE(spy.count(), SIGNAL);
}

void PlotDataProviderTest::checkRecomputeGroupingDataForColumnType(
    ColumnType columnType)
{
    PlotDataProvider provider;
    QSignalSpy spy(&provider, &PlotDataProvider::groupingPlotDataChanged);
    provider.recomputeGroupingData(calcData_, columnType);
    QCOMPARE(spy.count(), SIGNAL);
    checkSignalParamsForRecomputeGroupingAreEmpty(spy.first());
}

void PlotDataProviderTest::checkSignalParamsForRecomputeGroupingAreEmpty(
    const QList<QVariant>& signalParameters)
{
    QCOMPARE(signalParameters.size(), 3);
    QCOMPARE(signalParameters[0].value<QVector<int>>(), {});
    QCOMPARE(signalParameters[1].value<QVector<int>>(), {});
    QCOMPARE(signalParameters[2].value<Quantiles>().count_, Quantiles().count_);
}
