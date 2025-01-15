#include "PlotDataProvider.h"

#include <qwtble/QwtBleUtilities.h>
#include <QPointF>

void PlotDataProvider::recompute(QVector<TransactionData> newCalcData,
                                 ColumnType columnFormat)
{
    calcData_ = std::move(newCalcData);
    quantiles_ = computeQuantiles(calcData_);

    recomputeGroupingData(calcData_, columnFormat);

    auto [points, linearRegression]{computePointsAndRegression()};

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
                                             ColumnType columnFormat)
{
    calcData_ = std::move(calcData);

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
    const qsizetype dataSize{calcData.size()};

    // Group by name/string.
    for (int i{0}; i < dataSize; ++i)
        map[calcData.at(i).groupedBy_.toString()].append(
            calcData.at(i).pricePerMeter_);

    // For each group calculate quantiles and create names.
    auto iterator{map.begin()};
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
    const qsizetype dataSize{calcData_.size()};
    if (dataSize <= 0)
        return {{}, {}};

    double sumX{0.};
    double sumY{0.};
    double sumXX{0.};
    double sumXY{0.};
    double minX{0.};
    double maxX{0.};
    bool set = false;

    QVector<QPointF> data;
    data.reserve(dataSize);
    for (int i{0}; i < dataSize; ++i)
    {
        const QDate& date{calcData_.at(i).date_};
        const double x{static_cast<double>(
            qwt_ble_utilities::getStartOfTheWorld().daysTo(date))};
        auto y{calcData_.at(i).pricePerMeter_};
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
    const double a{(dataSize * sumXY - sumX * sumY) /
                   (dataSize * sumXX - sumX * sumX)};
    const double b{sumY / dataSize - a * sumX / dataSize};

    const QPointF linearRegressionFrom(minX, a * minX + b);
    const QPointF linearRegressionTo(maxX, a * maxX + b);
    QVector<QPointF> linearRegression;
    linearRegression.append(linearRegressionFrom);
    linearRegression.append(linearRegressionTo);

    return {data, linearRegression};
}

Quantiles PlotDataProvider::computeQuantiles(
    const QVector<TransactionData>& transactionData)
{
    Quantiles quantiles;
    if (const qsizetype dataSize{transactionData.size()}; dataSize != 0)
    {
        QVector<double> valuePerUnit{};
        valuePerUnit.reserve(dataSize);
        for (int i{0}; i < dataSize; ++i)
            valuePerUnit.push_back(transactionData.at(i).pricePerMeter_);
        quantiles.init(std::move(valuePerUnit));
    }
    return quantiles;
}
