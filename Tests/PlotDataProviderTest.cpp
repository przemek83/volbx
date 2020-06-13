#include "PlotDataProviderTest.h"

#include <QtTest/QtTest>

#include <PlotDataProvider.h>

PlotDataProviderTest::PlotDataProviderTest(QObject* parent) : QObject(parent) {}

void PlotDataProviderTest::initTestCase()
{
    mainQuantiles_.max_ = 15.;
    mainQuantiles_.min_ = 10.;
    mainQuantiles_.q10_ = 10.40;
    mainQuantiles_.q25_ = 11.;
    mainQuantiles_.q50_ = 12.;
    mainQuantiles_.q75_ = 13.50;
    mainQuantiles_.q90_ = 14.40;
    mainQuantiles_.mean_ = 12.33;
    mainQuantiles_.count_ = 3;
    mainQuantiles_.stdDev_ = 2.05;

    firstQuantiles_.max_ = 15.;
    firstQuantiles_.min_ = 10.;
    firstQuantiles_.q10_ = 10.40;
    firstQuantiles_.q25_ = 11.;
    firstQuantiles_.q50_ = 12.;
    firstQuantiles_.q75_ = 13.50;
    firstQuantiles_.q90_ = 14.40;
    firstQuantiles_.mean_ = 12.33;
    firstQuantiles_.count_ = 3;
    firstQuantiles_.stdDev_ = 2.05;

    secondQuantiles_.max_ = 5.;
    secondQuantiles_.min_ = 1.;
    secondQuantiles_.q10_ = 1.20;
    secondQuantiles_.q25_ = 1.5;
    secondQuantiles_.q50_ = 2.;
    secondQuantiles_.q75_ = 3.50;
    secondQuantiles_.q90_ = 4.40;
    secondQuantiles_.mean_ = 2.67;
    secondQuantiles_.count_ = 3;
    secondQuantiles_.stdDev_ = 1.70;
}

void PlotDataProviderTest::testRecomputeGroupingDataWrongColumnFormat()
{
    checkRecomputeGroupingDataForColumnType(ColumnType::DATE);
    checkRecomputeGroupingDataForColumnType(ColumnType::NUMBER);
    checkRecomputeGroupingDataForColumnType(ColumnType::UNKNOWN);
}

bool operator==(const Quantiles& left, const Quantiles& right)
{
    return left.getValuesAsToolTip() == right.getValuesAsToolTip();
}

void PlotDataProviderTest::testRecomputeGroupingData()
{
    PlotDataProvider provider;
    QSignalSpy spy(&provider, &PlotDataProvider::groupingPlotDataChanged);
    provider.recomputeGroupingData(calcData_, ColumnType::STRING);
    QCOMPARE(spy.count(), SIGNAL);
    QList<QVariant>& signalParameters{spy.first()};
    QCOMPARE(signalParameters.size(), 3);
    QCOMPARE(signalParameters[0].value<QVector<QString>>(),
             QVector({QString("column1"), QString("column2")}));
    QCOMPARE(signalParameters[1].value<QVector<Quantiles>>(),
             QVector({firstQuantiles_, secondQuantiles_}));
    QCOMPARE(signalParameters[2].value<Quantiles>(), Quantiles());
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
    QCOMPARE(signalParameters[0].value<QVector<QString>>(), {});
    QCOMPARE(signalParameters[1].value<QVector<Quantiles>>(), {});
    QCOMPARE(signalParameters[2].value<Quantiles>().count_, Quantiles().count_);
}
