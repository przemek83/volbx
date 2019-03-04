#ifndef QUANTILES_H
#define QUANTILES_H

#include <QVector>
#include <QString>

/**
 * @brief Quantiles struct. Computes, creates tooltip display, stores values.
 */
struct Quantiles
{
public:
    Quantiles();

    virtual ~Quantiles() = default;

    Quantiles& operator=(const Quantiles& other) = default;
    Quantiles(const Quantiles& other) = default;

    Quantiles& operator=(Quantiles&& other) = default;
    Quantiles(Quantiles&& other) = default;

    //Y axis variables.
    float min_;
    float q10_;
    float q25_;
    float q50_;
    float q75_;
    float q90_;
    float max_;

    int number_;
    float avg_;
    float stdDev_;

    //X axis variables.
    float minX_;
    float maxX_;

    void clear();

    void print();

    void computeQuantiles(QVector<float>& valuePerUnit);

    QString getValuesAsToolTip() const;

private:
    enum PlotInfo
    {
        PLOT_INFO_COUNT = 0,
        PLOT_INFO_MIN,
        PLOT_INFO_Q10,
        PLOT_INFO_Q25,
        PLOT_INFO_Q50,
        PLOT_INFO_Q75,
        PLOT_INFO_Q90,
        PLOT_INFO_MAX,
        PLOT_INFO_AVG,
        PLOT_INFO_STD_DEV,
        PLIT_INFO_END
    };

    static const char* plotInfoNames_[];

    static QString valueAsHtmlRow(PlotInfo name, double value);
};

#endif // QUANTILES_H
