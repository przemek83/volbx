#ifndef PLOTDATAPROVIDER_H
#define PLOTDATAPROVIDER_H

#include <ColumnType.h>
#include <Quantiles.h>
#include <QObject>

#include "Constants.h"

#include "TransactionData.h"

/**
 * @brief class used for computation of values for all plots.
 */
class PlotDataProvider : public QObject
{
    Q_OBJECT
public:
    explicit PlotDataProvider(QObject* parent = nullptr);

    ~PlotDataProvider() override = default;

    void setGroupingColumn(int groupingColumn);

    /**
     * @brief reCompute all data for plots.
     * @param newCalcData new data used for computations.
     * @param columnFormat format of grouping column.
     */
    void reCompute(QVector<TransactionData> newCalcData,
                   ColumnType columnFormat);

    /**
     * @brief recompute data for grouping plot.
     * @param calcData new data used for calculations.
     * @param groupingColumn number of column used for grouping.
     * @param columnFormat format of grouping column.
     */
    void recomputeGroupingData(QVector<TransactionData> calcData,
                               int groupingColumn, ColumnType columnFormat);

    /**
     * @brief get number of column used for grouping.
     * @return column number.
     */
    int getGroupByColumn();

Q_SIGNALS:
    void groupingPlotDataChanged(QVector<QString> intervalsNames,
                                 QVector<Quantiles> quantilesForIntervals,
                                 Quantiles quantiles);

    void basicPlotDataChanged(QVector<QPointF> data, Quantiles quantiles,
                              QVector<QPointF> linearRegression);

    void fundamentalDataChanged(QVector<double> data, Quantiles quantiles);

private:
    /**
     * @brief groups strings and for each group calculate quantiles and names.
     * @param calcData data used for calculations.
     * @param names vector where names will be added.
     * @param quantilesForIntervals vector where quantiles will be added.
     */
    std::tuple<QVector<QString>, QVector<Quantiles>> fillDataForStringGrouping(
        const QVector<TransactionData>& calcData);

    /**
     * @brief compute data used for simple plots (histogram and basic plots).
     */
    std::tuple<QVector<QPointF>, QVector<QPointF>> computePointsAndRegression();

    Quantiles computeQuantiles(const QVector<TransactionData>& transactionData);

    Quantiles quantiles_;

    QVector<TransactionData> calcData_;

    int groupingColumn_{Constants::NOT_SET_COLUMN};
};

#endif  // PLOTDATAPROVIDER_H
