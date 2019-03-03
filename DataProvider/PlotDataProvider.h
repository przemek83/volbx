#ifndef PLOTDATAPROVIDER_H
#define PLOTDATAPROVIDER_H

#include <QVector>
#include <Qmap>
#include <QDate>
#include <QVariant>
#include <QObject>
#include <qwt_series_data.h>

#include "Common/Quantiles.h"
#include "Common/Formats.h"
#include "DataProvider/TransactionData.h"
#include "Common/PlotData.h"

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
    explicit PlotDataProvider(int defaultGroupingColumn);

    ~PlotDataProvider() override = default;

    /**
     * @brief reCompute all data for plots.
     * @param newCalcData new data used for computations.
     * @param columnFormat format of grouping column.
     */
    void reCompute(QVector<TransactionData>* newCalcData,
                   DataFormat columnFormat);

    /**
     * @brief recompute data for grouping plot.
     * @param calcData new data used for calculations.
     * @param groupingColumn number of column used for grouping.
     * @param columnFormat format of grouping column.
     */
    void recomputeGroupData(QVector<TransactionData>* calcData,
                            int groupingColumn,
                            DataFormat columnFormat);

    /**
     * @brief get number of column used for grouping.
     * @return column number.
     */
    int getGroupByColumn();

private:
    Q_DISABLE_COPY(PlotDataProvider)

    void setNewCalcData(QVector<TransactionData>* calcData);

    /**
     * @brief groups strings and for each group calculate quantiles and names.
     * @param calcData data used for calculations.
     * @param names vector where names will be added.
     * @param quantilesForIntervals vectore where quantiles will be added.
     */
    void fillDataForStringGrouping(QVector<TransactionData>* calcData,
                                   QVector<QString>& names,
                                   QVector<Quantiles>& quantilesForIntervals);

    /**
     * @brief compute data used for simple plots (histogram and basic plots).
     */
    void computeBasicData();

    Quantiles quantiles_;

    QVector<TransactionData>* calcData_;

    ///Column used for grouping.
    int groupingColumn_;

signals:
    void setNewDataForGrouping(float minY,
                               float maxY,
                               QVector<QString> intervalsNames,
                               QVector<Quantiles> quantilesForIntervals,
                               Quantiles quantiles);

    void basicPlotDataChanged(PlotData plotData,
                              Quantiles quantiles,
                              QVector<QPointF> linearRegression);

    void basicDataChanged(PlotData plotData,
                          Quantiles quantiles);
};

#endif // PLOTDATAPROVIDER_H
