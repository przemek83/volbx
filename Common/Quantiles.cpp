#include "Quantiles.h"

#include <cmath>

#include <QDebug>

#include "Common/Constants.h"

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
    float EX = 0;
    float EX2 = 0;

    clear();

    number_ = valuePerUnit.count();

    for (float pricePerMeter : valuePerUnit)
    {
        if (min_ > pricePerMeter || Constants::floatsAreEqual(min_, 0.F))
        {
            min_ = pricePerMeter;
        }

        if (max_ < pricePerMeter || Constants::floatsAreEqual(max_, 0.F))
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
        stdDev_ = sqrtf(EX2 - EX * EX);
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
            float tmp = (number_ != 0 ? valuePerUnit.at(0) : 0);
            q10_ = tmp;
            q25_ = tmp;
            q50_ = tmp;
            q75_ = tmp;
            q90_ = tmp;
        }
        else
        {
            float tmp = (number_ - 1) * 0.1F;
            q10_ = valuePerUnit.at(static_cast<int>(floorf(tmp))) + (tmp - floorf(tmp)) *
                   (valuePerUnit.at(static_cast<int>(ceilf(tmp))) - valuePerUnit.at(static_cast<int>(floorf(tmp))));

            tmp = (number_ - 1) * 0.25F;
            q25_ = valuePerUnit.at(static_cast<int>(floorf(tmp))) + (tmp - floorf(tmp)) *
                   (valuePerUnit.at(static_cast<int>(ceilf(tmp))) - valuePerUnit.at(static_cast<int>(floorf(tmp))));

            tmp = (number_ - 1) * 0.5F;
            q50_ = valuePerUnit.at(static_cast<int>(floorf(tmp))) + (tmp - floorf(tmp)) *
                   (valuePerUnit.at(static_cast<int>(ceilf(tmp))) - valuePerUnit.at(static_cast<int>(floorf(tmp))));

            tmp = (number_ - 1) * 0.75F;
            q75_ = valuePerUnit.at(static_cast<int>(floorf(tmp))) + (tmp - floorf(tmp)) *
                   (valuePerUnit.at(static_cast<int>(ceilf(tmp))) - valuePerUnit.at(static_cast<int>(floorf(tmp))));

            tmp = (number_ - 1) * 0.9F;
            q90_ = valuePerUnit.at(static_cast<int>(floorf(tmp))) + (tmp - floorf(tmp)) *
                   (valuePerUnit.at(static_cast<int>(ceilf(tmp))) - valuePerUnit.at(static_cast<int>(floorf(tmp))));
        }
    }
}

QString Quantiles::getValuesAsToolTip() const
{
    QString toolTipText;
    toolTipText.append(QLatin1String("<table>"));
    toolTipText += valueAsHtmlRow(PLOT_INFO_COUNT, static_cast<float>(number_));
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

    toolTipText.append(QLatin1String("</table>"));

    return toolTipText;
}

QString Quantiles::valueAsHtmlRow(PlotInfo name, float value)
{
    static QString plotInfoNames_[PLIT_INFO_END];
    static bool initialized = false;

    if (!initialized)
    {
        plotInfoNames_[PLOT_INFO_COUNT] =
            QObject::tr("Data count") + QLatin1Char(':');
        plotInfoNames_[PLOT_INFO_MIN] = QStringLiteral("Minimum");
        plotInfoNames_[PLOT_INFO_Q10] = QStringLiteral("Q10");
        plotInfoNames_[PLOT_INFO_Q25] = QStringLiteral("Q25");
        plotInfoNames_[PLOT_INFO_Q50] = QStringLiteral("Q50");
        plotInfoNames_[PLOT_INFO_Q75] = QStringLiteral("Q75");
        plotInfoNames_[PLOT_INFO_Q90] = QStringLiteral("Q90");
        plotInfoNames_[PLOT_INFO_MAX] = QStringLiteral("Maximum");
        plotInfoNames_[PLOT_INFO_AVG] = QObject::tr("Average");
        plotInfoNames_[PLOT_INFO_STD_DEV] = QObject::tr("Std. deviation");
        initialized = true;
    }

    QString html(QStringLiteral("<tr><td>"));
    html.append(QString(plotInfoNames_[name]));
    html.append(QLatin1String("</td><td ALIGN=RIGHT>"));
    int precision = (PLOT_INFO_COUNT == name ? 0 : 2);
    html.append(Constants::floatToStringUsingLocale(value, precision));
    html.append(QLatin1String("</td></tr>\n"));

    return html;
}
