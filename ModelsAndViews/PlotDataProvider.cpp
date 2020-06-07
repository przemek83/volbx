#include "PlotDataProvider.h"

#include <algorithm>
#include <cmath>

#include <QwtBleUtilities.h>
#include <QApplication>
#include <QDebug>
#include <QLocale>
#include <QVariant>
#include <QtCore>

#include "Shared/Logger.h"

PlotDataProvider::PlotDataProvider(QObject* parent) : QObject(parent) {}

void PlotDataProvider::setGroupingColumn(int groupingColumn)
{
    groupingColumn_ = groupingColumn;
}

void PlotDataProvider::reCompute(QVector<TransactionData> newCalcData,
                                 ColumnType columnFormat)
{
    calcData_ = std::move(newCalcData);

    quantiles_.clear();

    int dataSize = calcData_.size();
    if (dataSize != 0)
    {
        QVector<double> valuePerUnit{};
        valuePerUnit.reserve(dataSize);
        for (int i = 0; i < dataSize; ++i)
            valuePerUnit.push_back(calcData_.at(i).pricePerMeter_);
        quantiles_.init(std::move(valuePerUnit));
    }

    // Left part of group plot.
    recomputeGroupData(calcData_, groupingColumn_, columnFormat);

    auto [plotData, linearRegression] = computeBasicData();

    QVector<double> yAxisData;
    yAxisData.reserve(plotData.size());
    for (const auto& point : qAsConst(plotData))
        yAxisData.append(point.y());

    // Basic data plot.
    Q_EMIT basicPlotDataChanged(std::move(plotData), quantiles_,
                                std::move(linearRegression));

    // Currently histogram only.
    Q_EMIT basicDataChanged(std::move(yAxisData), quantiles_);
}

void PlotDataProvider::recomputeGroupData(QVector<TransactionData> calcData,
                                          int groupingColumn,
                                          ColumnType columnFormat)
{
    calcData_ = std::move(calcData);

    // Set grouping column. Different only than actual one when changed on
    // group plot.
    groupingColumn_ = groupingColumn;

    // Remove when other column types will be managed.
    if (groupingColumn_ == Constants::NOT_SET_COLUMN)
        return;

    QVector<QString> names;
    QVector<Quantiles> quantilesForIntervals;

    // For now only string type columns managed.
    if (ColumnType::STRING == columnFormat)
        fillDataForStringGrouping(calcData_, names, quantilesForIntervals);

    Q_EMIT setNewDataForGrouping(std::move(names),
                                 std::move(quantilesForIntervals), quantiles_);
}

void PlotDataProvider::fillDataForStringGrouping(
    const QVector<TransactionData>& calcData, QVector<QString>& names,
    QVector<Quantiles>& quantilesForIntervals)
{
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
}

std::tuple<QVector<QPointF>, QVector<QPointF>>
PlotDataProvider::computeBasicData()
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

int PlotDataProvider::getGroupByColumn() { return groupingColumn_; }
