#include "PlotDataProvider.h"

#include <algorithm>
#include <cmath>

#include <QApplication>
#include <QDebug>
#include <QLocale>
#include <QVariant>
#include <QtCore>

#include "Common/Constants.h"
#include "Common/ProgressBar.h"
#include "Shared/Logger.h"

PlotDataProvider::PlotDataProvider(QObject* parent) :
    QObject(parent)
{

}

void PlotDataProvider::setGroupingColumn(int groupingColumn)
{
    groupingColumn_ = groupingColumn;
}

void PlotDataProvider::reCompute(QVector<TransactionData> newCalcData,
                                 DataFormat columnFormat)
{
    calcData_ = std::move(newCalcData);

    QVector<float> valuePerUnit;
    quantiles_.clear();

    int dataSize = calcData_.size();

    if (0 != dataSize)
    {
        for (int i = 0; i < dataSize; ++i)
        {
            valuePerUnit.push_back(calcData_.at(i).pricePerMeter_);
        }

        quantiles_.computeQuantiles(valuePerUnit);
    }

    //Left part of group plot.
    recomputeGroupData(calcData_, groupingColumn_, columnFormat);

    //Basic data plot.
    computeBasicData();
}

void PlotDataProvider::recomputeGroupData(QVector<TransactionData> calcData,
                                          int groupingColumn,
                                          DataFormat columnFormat)
{
    calcData_ = std::move(calcData);

    //Set grouping column. Different only than actual one when changed on
    //group plot.
    groupingColumn_ = groupingColumn;

    //Remove when other column types will be managed.
    if (-1 == groupingColumn_)
    {
        return;
    }

    QVector<QString> names;
    QVector<Quantiles> quantilesForIntervals;

    //For now only string type columns managed.
    if (DATA_FORMAT_STRING == columnFormat)
    {
        fillDataForStringGrouping(calcData_, names, quantilesForIntervals);
    }

    Q_EMIT setNewDataForGrouping(quantiles_.min_, quantiles_.max_, names,
                                 quantilesForIntervals, quantiles_);
}

void PlotDataProvider::fillDataForStringGrouping(const QVector<TransactionData>& calcData,
                                                 QVector<QString>& names,
                                                 QVector<Quantiles>& quantilesForIntervals)
{
    QMap<QString, QVector<float> > map;

    int dataSize = calcData.size();

    //Group by name/string.
    for (int i = 0; i < dataSize; ++i)
    {
        map[calcData.at(i).groupedBy_.toString()].append(calcData.at(i).pricePerMeter_);
    }

    //For each group calculate quantiles and create names.
    QMap<QString, QVector<float> >::iterator iterator = map.begin();
    while (iterator != map.end())
    {
        names.append(iterator.key());
        Quantiles quantiles;
        quantiles.computeQuantiles(iterator.value());
        quantilesForIntervals.append(quantiles);
        ++iterator;
    }
}

void PlotDataProvider::computeBasicData()
{
    int dataSize = calcData_.size();
    if (dataSize <= 0)
    {
        PlotData plotData(nullptr, nullptr, 0);

        Q_EMIT basicPlotDataChanged(plotData, quantiles_,
                                    QVector<QPointF>());

        return;
    }

    //Create points for quantiles - x, y, min, max.
    auto pointsQuantilesX = new double[static_cast<unsigned long long>(dataSize)];
    auto pointsQuantilesY = new double[static_cast<unsigned long long>(dataSize)];

    double sumX = 0.;
    double sumY = 0.;
    double sumXX = 0.;
    double sumXY = 0.;
    double a = 0.;
    double b = 0.;

    double minX = 0.;
    double maxX = 0.;
    bool set = false;

    for (int i = 0; i < dataSize; ++i)
    {
        double x = Constants::startOfTheWorld.daysTo(calcData_.at(i).date_);
        double y = static_cast<double>(calcData_.at(i).pricePerMeter_);
        pointsQuantilesX[i] = x;
        pointsQuantilesY[i] = y;

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
            {
                minX = x;
            }
            if (maxX < x)
            {
                maxX = x;
            }
        }
    }

    quantiles_.minX_ = static_cast<float>(minX);
    quantiles_.maxX_ = static_cast<float>(maxX);

    //Calc linear regression and create points.
    a = (dataSize * sumXY - sumX * sumY) / (dataSize * sumXX - sumX * sumX);
    b = sumY / dataSize - a * sumX / dataSize;

    QPointF linearRegressionFrom(minX, a * minX + b);
    QPointF linearRegressionTo(maxX, a * maxX + b);
    QVector<QPointF> linearRegression;
    linearRegression.append(linearRegressionFrom);
    linearRegression.append(linearRegressionTo);

    //Create plot data and propagate it.
    PlotData plotData(pointsQuantilesX, pointsQuantilesY, dataSize);

    //Basic data plot.
    Q_EMIT basicPlotDataChanged(plotData, quantiles_, linearRegression);

    //Currently histogram only.
    Q_EMIT basicDataChanged(plotData,
                            quantiles_);
}

int PlotDataProvider::getGroupByColumn()
{
    return groupingColumn_;
}
