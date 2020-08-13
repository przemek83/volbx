#include "PlotDataProviderTest.h"

#include <QtTest/QtTest>

#include <PlotDataProvider.h>

void PlotDataProviderTest::initTestCase()
{
    mainQuantiles_.max_ = 15.;
    mainQuantiles_.min_ = 1.;
    mainQuantiles_.q10_ = 1.50;
    mainQuantiles_.q25_ = 2.75;
    mainQuantiles_.q50_ = 7.50;
    mainQuantiles_.q75_ = 11.50;
    mainQuantiles_.q90_ = 13.50;
    mainQuantiles_.mean_ = 7.50;
    mainQuantiles_.count_ = 6;
    mainQuantiles_.stdDev_ = 5.19;

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

void PlotDataProviderTest::testRecomputeGroupingIvalidFormat()
{
    for (auto columnType :
         {ColumnType::DATE, ColumnType::NUMBER, ColumnType::UNKNOWN})
        checkRecomputeGroupingDataForColumnType(columnType);
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

    // General Quantiles data is empty as recompute() was not called.
    checkGroupingDataChangedSignal(
        spy, QVector{QString("column1"), QString("column2")},
        QVector{firstQuantiles_, secondQuantiles_}, Quantiles());
}

void PlotDataProviderTest::testRecomputeGroupingDataEmptyCalcData()
{
    PlotDataProvider provider;
    QSignalSpy spy(&provider, &PlotDataProvider::groupingPlotDataChanged);
    provider.recomputeGroupingData({}, ColumnType::STRING);

    checkGroupingDataChangedSignal(spy, {}, {}, Quantiles());
}

void PlotDataProviderTest::testRecompute_data()
{
    QTest::addColumn<QVector<TransactionData>>("calcData");
    QTest::addColumn<Quantiles>("quantiles");
    QTest::addColumn<QVector<QString>>("intervalsNames");
    QTest::addColumn<QVector<Quantiles>>("quantilesForIntervals");
    QTest::addColumn<QVector<QPointF>>("points");
    QTest::addColumn<QVector<QPointF>>("regression");
    QTest::addColumn<QVector<double>>("yAxisValues");

    QTest::newRow("Test recompute empty data")
        << QVector<TransactionData>() << Quantiles() << QVector<QString>()
        << QVector<Quantiles>() << QVector<QPointF>() << QVector<QPointF>()
        << QVector<double>();

    QTest::newRow("Test recompute")
        << calcData_ << mainQuantiles_
        << QVector{QString("column1"), QString("column2")}
        << QVector{firstQuantiles_, secondQuantiles_} << points_ << regression_
        << yAxisValues_;
}

void PlotDataProviderTest::testRecompute()
{
    QFETCH(QVector<TransactionData>, calcData);
    QFETCH(Quantiles, quantiles);
    QFETCH(QVector<QString>, intervalsNames);
    QFETCH(QVector<Quantiles>, quantilesForIntervals);
    QFETCH(QVector<QPointF>, points);
    QFETCH(QVector<QPointF>, regression);
    QFETCH(QVector<double>, yAxisValues);

    PlotDataProvider provider;
    QSignalSpy groupingPlotDataChangedSpy(
        &provider, &PlotDataProvider::groupingPlotDataChanged);
    QSignalSpy basicPlotDataChangedSpy(&provider,
                                       &PlotDataProvider::basicPlotDataChanged);
    QSignalSpy fundamentalDataChangedSpy(
        &provider, &PlotDataProvider::fundamentalDataChanged);
    provider.recompute(calcData, ColumnType::STRING);

    checkGroupingDataChangedSignal(groupingPlotDataChangedSpy, intervalsNames,
                                   quantilesForIntervals, quantiles);

    checkBasicDataChangedSignal(basicPlotDataChangedSpy, points, quantiles,
                                regression);

    checkFundamentalDataChangedSignal(fundamentalDataChangedSpy, yAxisValues,
                                      quantiles);
}

void PlotDataProviderTest::checkRecomputeGroupingDataForColumnType(
    ColumnType columnType)
{
    PlotDataProvider provider;
    QSignalSpy spy(&provider, &PlotDataProvider::groupingPlotDataChanged);
    provider.recomputeGroupingData(calcData_, columnType);
    checkGroupingDataChangedSignal(spy, {}, {}, Quantiles());
}

void PlotDataProviderTest::checkGroupingDataChangedSignal(
    const QSignalSpy& spy, const QVector<QString>& expectedIntervalsNames,
    const QVector<Quantiles>& expectedQuantilesForIntervals,
    const Quantiles& expectedQuantiles)
{
    QCOMPARE(spy.count(), SIGNAL);
    const QList<QVariant>& signalParameters{spy.first()};
    QCOMPARE(signalParameters.size(), 3);
    QCOMPARE(signalParameters[0].value<QVector<QString>>(),
             expectedIntervalsNames);
    QCOMPARE(signalParameters[1].value<QVector<Quantiles>>(),
             expectedQuantilesForIntervals);
    QCOMPARE(signalParameters[2].value<Quantiles>(), expectedQuantiles);
}

void PlotDataProviderTest::checkBasicDataChangedSignal(
    const QSignalSpy& spy, const QVector<QPointF>& expectedPoints,
    const Quantiles& expectedQuantiles,
    const QVector<QPointF>& expectedRegression)
{
    QCOMPARE(spy.count(), SIGNAL);
    const QList<QVariant>& signalParameters{spy.first()};
    QCOMPARE(signalParameters.size(), 3);
    QCOMPARE(signalParameters[0].value<QVector<QPointF>>(), expectedPoints);
    QCOMPARE(signalParameters[1].value<Quantiles>(), expectedQuantiles);
    QCOMPARE(signalParameters[2].value<QVector<QPointF>>(), expectedRegression);
}

void PlotDataProviderTest::checkFundamentalDataChangedSignal(
    const QSignalSpy& spy, const QVector<double>& expectedYAxisValues,
    const Quantiles& expectedQuantiles)
{
    QCOMPARE(spy.count(), SIGNAL);
    const QList<QVariant>& signalParameters{spy.first()};
    QCOMPARE(signalParameters.size(), 2);
    QCOMPARE(signalParameters[0].value<QVector<double>>(), expectedYAxisValues);
    QCOMPARE(signalParameters[1].value<Quantiles>(), expectedQuantiles);
}
