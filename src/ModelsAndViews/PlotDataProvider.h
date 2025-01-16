#pragma once

#include <eible/ColumnType.h>
#include <qwtble/Quantiles.h>
#include <QObject>

#include "TransactionData.h"

class PlotDataProvider : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;

    void recompute(QVector<TransactionData> newCalcData,
                   ColumnType columnFormat);

    void recomputeGroupingData(QVector<TransactionData> calcData,
                               ColumnType columnFormat);

signals:
    void groupingPlotDataChanged(QVector<QString> intervalsNames,
                                 QVector<Quantiles> quantilesForIntervals,
                                 Quantiles quantiles);

    void basicPlotDataChanged(QVector<QPointF> data, Quantiles quantiles,
                              QVector<QPointF> linearRegression);

    void fundamentalDataChanged(QVector<double> data, Quantiles quantiles);

private:
    /// Groups strings and for each group calculate quantiles and names.
    static std::tuple<QVector<QString>, QVector<Quantiles>>
    fillDataForStringGrouping(const QVector<TransactionData>& calcData);

    /// Compute data used for simple plots (histogram and basic plots).
    std::tuple<QVector<QPointF>, QVector<QPointF>> computePointsAndRegression();

    static Quantiles computeQuantiles(
        const QVector<TransactionData>& transactionData);

    Quantiles quantiles_;

    QVector<TransactionData> calcData_;
};
