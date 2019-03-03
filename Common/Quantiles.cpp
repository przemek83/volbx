#include <math.h>

#include <QDebug>

#include "Common/Constants.h"

#include "Quantiles.h"

Quantiles::Quantiles()
{
    clear();
}

void Quantiles::clear()
{
    min_ = .0;
    q10_ = .0;
    q25_ = .0;
    q50_ = .0;
    q75_ = .0;
    q90_ = .0;
    max_ = .0;

    number_ = 0;
    avg_ = .0;
    stdDev_ = .0;

    minX_ = .0;
    maxX_ = .0;
}

void Quantiles::print()
{
    qDebug() << "min " << min_ << " q10 = " << q10_ << " q25_ = " << q25_ << \
             " q50 = " << q50_ << " q75 = " << q75_ << " q90 = " << q90_ << \
             " max = " << max_ << " number = " << number_ << " avg = " << \
             avg_ << " std dev = " << stdDev_ <<  " minX = " << minX_ << \
             " maxX = " << maxX_;
}

void Quantiles::computeQuantiles(QVector<float>& valuePerUnit)
{
    float EX = 0, EX2 = 0;

    clear();

    number_ = valuePerUnit.count();

    for (float pricePerMeter : valuePerUnit)
    {
        if (min_ > pricePerMeter || 0 == min_)
        {
            min_ = pricePerMeter;
        }

        if (max_ < pricePerMeter || 0 == max_)
        {
            max_ = pricePerMeter;
        }

        EX += pricePerMeter;
        EX2 += pricePerMeter * pricePerMeter;
    }

    EX /= number_;
    EX2 /= number_;

    if (number_ > 1)
    {
        stdDev_ = sqrt(EX2 - EX * EX);
    }
    else
    {
        stdDev_ = .0;
    }

    avg_ = EX;


    if (stdDev_ > 0)
    {
        qSort(valuePerUnit);

        if (number_ < 2)
        {
            float tmp = (number_ ? valuePerUnit.at(0) : 0);
            q10_ = tmp;
            q25_ = tmp;
            q50_ = tmp;
            q75_ = tmp;
            q90_ = tmp;
        }
        else
        {
            float tmp = (number_ - 1) * 0.1;
            q10_ = valuePerUnit.at(floor(tmp)) + (tmp - floor(tmp)) *
                   (valuePerUnit.at(ceil(tmp)) - valuePerUnit.at(floor(tmp)));

            tmp = (number_ - 1) * 0.25;
            q25_ = valuePerUnit.at(floor(tmp)) + (tmp - floor(tmp)) *
                   (valuePerUnit.at(ceil(tmp)) - valuePerUnit.at(floor(tmp)));

            tmp = (number_ - 1) * 0.5;
            q50_ = valuePerUnit.at(floor(tmp)) + (tmp - floor(tmp)) *
                   (valuePerUnit.at(ceil(tmp)) - valuePerUnit.at(floor(tmp)));

            tmp = (number_ - 1) * 0.75;
            q75_ = valuePerUnit.at(floor(tmp)) + (tmp - floor(tmp)) *
                   (valuePerUnit.at(ceil(tmp)) - valuePerUnit.at(floor(tmp)));

            tmp = (number_ - 1) * 0.9;
            q90_ = valuePerUnit.at(floor(tmp)) + (tmp - floor(tmp)) *
                   (valuePerUnit.at(ceil(tmp)) - valuePerUnit.at(floor(tmp)));
        }
    }
}

QString Quantiles::getValuesAsToolTip() const
{
    QString toolTipText;
    toolTipText.append("<table>");
    toolTipText += valueAsHtmlRow(PLOT_INFO_COUNT, static_cast<double>(number_));
    toolTipText += valueAsHtmlRow(PLOT_INFO_AVG, avg_);
    toolTipText += valueAsHtmlRow(PLOT_INFO_MAX, max_);

    if (number_ > 1)
    {
        toolTipText += valueAsHtmlRow(PLOT_INFO_Q90, q90_);
        toolTipText += valueAsHtmlRow(PLOT_INFO_Q75, q75_);
        toolTipText += valueAsHtmlRow(PLOT_INFO_Q50, q50_);
        toolTipText += valueAsHtmlRow(PLOT_INFO_Q25, q25_);
        toolTipText += valueAsHtmlRow(PLOT_INFO_Q10, q10_);
    }

    toolTipText += valueAsHtmlRow(PLOT_INFO_MIN, min_);

    if (number_ > 1)
    {
        toolTipText += valueAsHtmlRow(PLOT_INFO_STD_DEV, stdDev_);
    }

    toolTipText.append("</table>");

    return toolTipText;
}

QString Quantiles::valueAsHtmlRow(PlotInfo row, double value)
{
    static QString plotInfoNames_[PLIT_INFO_END];
    static bool initialized = false;

    if (false == initialized)
    {
        plotInfoNames_[PLOT_INFO_COUNT] =
            QObject::tr("Data count") + QLatin1Char(':');
        plotInfoNames_[PLOT_INFO_MIN] = "Minimum";
        plotInfoNames_[PLOT_INFO_Q10] = "Q10";
        plotInfoNames_[PLOT_INFO_Q25] = "Q25";
        plotInfoNames_[PLOT_INFO_Q50] = "Q50";
        plotInfoNames_[PLOT_INFO_Q75] = "Q75";
        plotInfoNames_[PLOT_INFO_Q90] = "Q90";
        plotInfoNames_[PLOT_INFO_MAX] = "Maximum";
        plotInfoNames_[PLOT_INFO_AVG] = QObject::tr("Average");
        plotInfoNames_[PLOT_INFO_STD_DEV] = QObject::tr("Std. deviation");
        initialized = true;
    }

    QString html("<tr><td>");
    html.append(QString(plotInfoNames_[row]));
    html.append("</td><td ALIGN=RIGHT>");
    int precision = (PLOT_INFO_COUNT == row ? 0 : 2);
    html.append(Constants::floatToStringUsingLocale(value, precision));
    html.append("</td></tr>\n");

    return html;
}
