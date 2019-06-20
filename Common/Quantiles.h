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
    Quantiles() = default;

    ~Quantiles() = default;

    Quantiles& operator=(const Quantiles& other) = default;
    Quantiles(const Quantiles& other) = default;

    Quantiles& operator=(Quantiles&& other) = default;
    Quantiles(Quantiles&& other) = default;

    //Y axis variables.
    float min_ {.0};
    float q10_ {.0};
    float q25_ {.0};
    float q50_ {.0};
    float q75_ {.0};
    float q90_ {.0};
    float max_ {.0};

    int number_ {0};
    float avg_ {.0};
    float stdDev_ {.0};

    //X axis variables.
    float minX_ {.0};
    float maxX_ {.0};

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

    static QString valueAsHtmlRow(PlotInfo name, float value);
};

#endif // QUANTILES_H
