#include "PlotDataProvider.h"

#include <QwtBleUtilities.h>
#include <QPointF>

PlotDataProvider::PlotDataProvider(QObject* parent) : QObject(parent) {}

void PlotDataProvider::setGroupingColumn(int groupingColumn)
{
    groupingColumn_ = groupingColumn;
}

void PlotDataProvider::reCompute(QVector<TransactionData> newCalcData,
                                 ColumnType columnFormat)
{
    calcData_ = std::move(newCalcData);
    quantiles_ = computeQuantiles(calcData_);

    recomputeGroupingData(calcData_, groupingColumn_, columnFormat);

    auto [points, linearRegression] = computePointsAndRegression();

    QVector<double> yAxisValues;
    yAxisValues.reserve(points.size());
    for (const auto& point : qAsConst(points))
        yAxisValues.append(point.y());

    Q_EMIT basicPlotDataChanged(std::move(points), quantiles_,
                                std::move(linearRegression));

    // Currently only histogram plot is attached under this signal.
    Q_EMIT fundamentalDataChanged(std::move(yAxisValues), quantiles_);
}

void PlotDataProvider::recomputeGroupingData(QVector<TransactionData> calcData,
                                             int groupingColumn,
                                             ColumnType columnFormat)
{
    calcData_ = std::move(calcData);
    groupingColumn_ = groupingColumn;
    if (groupingColumn_ == Constants::NOT_SET_COLUMN)
        return;

    if (ColumnType::STRING != columnFormat)
    {
        Q_EMIT groupingPlotDataChanged({}, {}, quantiles_);
        return;
    }

    auto [names, quantilesForIntervals] = fillDataForStringGrouping(calcData_);
    Q_EMIT groupingPlotDataChanged(
        std::move(names), std::move(quantilesForIntervals), quantiles_);
}

std::tuple<QVector<QString>, QVector<Quantiles>>
PlotDataProvider::fillDataForStringGrouping(
    const QVector<TransactionData>& calcData)
{
    QVector<QString> names;
    QVector<Quantiles> quantilesForIntervals;
    QMap<QString, QVector<double>> map;
    const int dataSize = calcData.size();

    // Group by name/string.
    for (int i = 0; i < dataSize; ++i)
        map[calcData.at(i).groupedBy_.toString()].append(
            calcData.at(i).pricePerMeter_);

    // For each group calculate quantiles and create names.
    auto iterator = map.begin();
    while (iterator != map.end())
    {
        names.append(iterator.key());
        Quantiles quantiles;
        quantiles.init(iterator.value());
        quantilesForIntervals.append(quantiles);
        ++iterator;
    }

    return {names, quantilesForIntervals};
}

std::tuple<QVector<QPointF>, QVector<QPointF>>
PlotDataProvider::computePointsAndRegression()
{
    int dataSize = calcData_.size();
    if (dataSize <= 0)
        return {{}, {}};

    double sumX = 0.;
    double sumY = 0.;
    double sumXX = 0.;
    double sumXY = 0.;
    double a = 0.;
    double b = 0.;

    double minX = 0.;
    double maxX = 0.;
    bool set = false;

    QVector<QPointF> data;
    data.reserve(dataSize);
    for (int i = 0; i < dataSize; ++i)
    {
        double x =
            QwtBleUtilities::getStartOfTheWorld().daysTo(calcData_.at(i).date_);
        auto y = calcData_.at(i).pricePerMeter_;
        data.append({x, y});

        sumX += x;
        sumY += y;
        sumXX += x * x;
        sumXY += x * y;

        if (!set)
        {
            set = true;
            minX = x;
            maxX = x;
        }
        else
        {
            if (minX > x)
                minX = x;
            if (maxX < x)
                maxX = x;
        }
    }

    quantiles_.minX_ = minX;
    quantiles_.maxX_ = maxX;

    // Calc linear regression and create points.
    a = (dataSize * sumXY - sumX * sumY) / (dataSize * sumXX - sumX * sumX);
    b = sumY / dataSize - a * sumX / dataSize;

    QPointF linearRegressionFrom(minX, a * minX + b);
    QPointF linearRegressionTo(maxX, a * maxX + b);
    QVector<QPointF> linearRegression;
    linearRegression.append(linearRegressionFrom);
    linearRegression.append(linearRegressionTo);

    return {data, linearRegression};
}

Quantiles PlotDataProvider::computeQuantiles(
    const QVector<TransactionData>& transactionData)
{
    Quantiles quantiles;
    int dataSize = transactionData.size();
    if (dataSize != 0)
    {
        QVector<double> valuePerUnit{};
        valuePerUnit.reserve(dataSize);
        for (int i = 0; i < dataSize; ++i)
            valuePerUnit.push_back(transactionData.at(i).pricePerMeter_);
        quantiles.init(std::move(valuePerUnit));
    }
    return quantiles;
}

int PlotDataProvider::getGroupByColumn() { return groupingColumn_; }
