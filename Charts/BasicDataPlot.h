#ifndef BASICDATAPLOT_H
#define BASICDATAPLOT_H

#include <qwt_plot.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_picker.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_draw.h>

#include "Common/Quantiles.h"
#include "Common/PlotData.h"

#include "PlotBase.h"
#include "Picker.h"

class QMouseEvent;

/**
 * @brief Linear regression plot with data and quantile 25, 50 and 75.
 */
class BasicDataPlot : public PlotBase
{
    Q_OBJECT
public:
    explicit BasicDataPlot(QWidget* parent = nullptr);

    virtual ~BasicDataPlot();

public slots:
    virtual void setNewData(PlotData plotData,
                            Quantiles quantiles,
                            QVector<QPointF> linearRegression);

private:
    Q_DISABLE_COPY(BasicDataPlot)

    void initPlotCurve();
    void initQ25();
    void initQ50();
    void initQ75();
    void initLinearRegression();
    void initLegend();

    QwtPlotCurve plotQ25_;
    QwtPlotCurve plotQ50_;
    QwtPlotCurve plotQ75_;
    QwtPlotCurve plotLinearRegression_;

    class TimeScaleDraw: public QwtScaleDraw
    {
    public:
        TimeScaleDraw();

        virtual ~TimeScaleDraw();

        virtual QwtText label(double v) const;
    };

    class LinearPicker : public Picker
    {
    public:
        explicit LinearPicker(QWidget* parent);

        virtual ~LinearPicker();

    protected:
        virtual QwtText trackerTextF(const QPointF &pos)const;
    };

    LinearPicker* picker_;

    /**
     * @brief set given item checked on legend.
     * @param plot plot related to item on legend.
     */
    void setLegendItemChecked(QwtPlotCurve* plot);

private slots:
    void legendItemChecked(QVariant itemInfo, bool on, int index);
};

#endif // BASICDATAPLOT_H
