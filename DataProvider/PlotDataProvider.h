#ifndef PLOTDATAPROVIDER_H
#define PLOTDATAPROVIDER_H

#include <PlotData.h>
#include <QDate>
#include <QMap>
#include <QObject>
#include <Quantiles.h>
#include <qwt_series_data.h>
#include <QVariant>
#include <QVector>

#include "Common/Constants.h"
#include "Common/Formats.h"
#include "DataProvider/TransactionData.h"

class QwtIntervalSeriesData;
class QwtPointSeriesData;
class QDate;

/**
 * @brief class used for computation of values for all plots.
 */
class PlotDataProvider : public QObject
{
    Q_OBJECT
public:
    explicit PlotDataProvider(QObject* parent);

    ~PlotDataProvider() override = default;

    PlotDataProvider& operator=(const PlotDataProvider& other) = delete;
    PlotDataProvider(const PlotDataProvider& other) = delete;

    PlotDataProvider& operator=(PlotDataProvider&& other) = delete;
    PlotDataProvider(PlotDataProvider&& other) = delete;

    void setGroupingColumn(int groupingColumn);

    /**
     * @brief reCompute all data for plots.
     * @param newCalcData new data used for computations.
     * @param columnFormat format of grouping column.
     */
    void reCompute(QVector<TransactionData> newCalcData,
                   DataFormat columnFormat);

    /**
     * @brief recompute data for grouping plot.
     * @param calcData new data used for calculations.
     * @param groupingColumn number of column used for grouping.
     * @param columnFormat format of grouping column.
     */
    void recomputeGroupData(QVector<TransactionData> calcData,
                            int groupingColumn,
                            DataFormat columnFormat);

    /**
     * @brief get number of column used for grouping.
     * @return column number.
     */
    int getGroupByColumn();

private:
    void setNewCalcData(QVector<TransactionData> calcData);

    /**
     * @brief groups strings and for each group calculate quantiles and names.
     * @param calcData data used for calculations.
     * @param names vector where names will be added.
     * @param quantilesForIntervals vectore where quantiles will be added.
     */
    void fillDataForStringGrouping(const QVector<TransactionData>& calcData,
                                   QVector<QString>& names,
                                   QVector<Quantiles>& quantilesForIntervals);

    /**
     * @brief compute data used for simple plots (histogram and basic plots).
     */
    std::tuple<QVector<QPointF>, QVector<QPointF>> computeBasicData();

    Quantiles quantiles_;

    QVector<TransactionData> calcData_;

    ///Column used for grouping.
    int groupingColumn_ {Constants::NOT_SET_COLUMN};

Q_SIGNALS:
    void setNewDataForGrouping(QVector<QString> intervalsNames,
                               QVector<Quantiles> quantilesForIntervals,
                               Quantiles quantiles);

    void basicPlotDataChanged(QVector<QPointF> data,
                              Quantiles quantiles,
                              QVector<QPointF> linearRegression);

    void basicDataChanged(QVector<double> data,
                          Quantiles quantiles);
};

#endif // PLOTDATAPROVIDER_H
